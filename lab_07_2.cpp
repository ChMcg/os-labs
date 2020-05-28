#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

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
    int filedes = open("my_pipe", O_RDONLY | O_NONBLOCK);
    if (filedes == -1)
    {
        perror("open");
        return (void *)(-1);
    }
    int status;
    char letter[1];
    while (temp->work)
    {
        status = read(filedes, (void *)letter, 1);
        if (status == -1)
            perror("read ");
        else 
            if (status == 0)
                cout << "Прочитано: канал пуст" << endl;
            else
                cout << "Прочитано " << letter << endl;
        cout.flush();
        sleep(1);
    }
    close(filedes);
    cout << "Поток 1 завершил работу" << endl;
    cout.flush();
    return 0;
}


int main()
{
    cout.setf(ios::unitbuf);
    pthread_t f1;
    p_args args;
    args.work = true;
    // signal(SIGPIPE, SIG_IGN);
    int status = 0;
    status = mkfifo("my_pipe", S_IRUSR| S_IWUSR);
    if (status == -1)
        perror("mkfifo : ");
    status = pthread_create(&f1, 0, some_p1, &args);
    if (!status)
        cout << "Поток 1 создан\n" << endl;
    else
        cout << "Ошибка создания потока: " << status << endl;
    getch();
    args.work = false;
    pthread_join(f1, 0);
    cout << "Поток 1 остановлен" << endl;
    unlink("my_pipe");
    return 0;
}