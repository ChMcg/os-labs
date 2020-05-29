#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>

#include "getch.cpp"

using namespace std;

struct p_args
{
    bool work;
};

void *some_p1(void *arg)
{
    cout.setf(ios::unitbuf);
    cout << "Поток 1 начал работу" << endl;
    p_args *temp = (p_args *)arg;
    struct stat file_status;
    int filedes = -1;
    int status;
    char data[1];
    data[0] = 'A';

    while (temp->work)
    {
        if (filedes == -1)
            filedes = open("my_pipe", O_WRONLY | O_NONBLOCK);
        if (filedes == -1)
        {
            // perror("open");
            cout << "Канал не готов к записи" << endl;
            sleep(1);
            continue;
        }
        else 
        {

            if (data[0] == 'D') data[0] = 'A';
            status = write(filedes, data, 1);
            if (status == -1) 
            {
                cout << "Канал не готов к записи" << endl;
            }
            else cout << "Записано: " << data << endl;
            cout.flush();
            data[0] += 1;
            sleep(1);
        }
    }
    close(filedes);
    cout << "Поток 1 закончил работу" << endl;
    return 0;
}


int main()
{
    cout.setf(ios::unitbuf);
    pthread_t f1;
    p_args args;
    args.work = true;
    signal(SIGPIPE, SIG_IGN);
    int status = 0;
    status = mkfifo("my_pipe", S_IRUSR| S_IWUSR);
    if (status == -1)
        perror("mkfifo");
    status = pthread_create(&f1, 0, some_p1, &args);
    if (!status)
        cout << "Поток 1 создан" << endl;
    else
        cout << "Ошибка создания потока: " << status << endl;
    getch();
    args.work = false;
    pthread_join(f1, 0);
    cout << "Поток 1 остановлен" << endl;
    // unlink("my_pipe");

    return 0;
}