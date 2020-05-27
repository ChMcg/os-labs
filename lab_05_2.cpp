#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include "getch.cpp"

using namespace std;

struct p_args
{
    bool work;
    char fname[25] = "./out.txt";
    sem_t *sem;
};

void *some_p1(void *arg)
{
    cout << "Поток 1 начал работу" << endl;
    p_args *temp = (p_args *)arg;
    ofstream out_file;
    out_file.open(temp->fname, ios::app);
    if (!out_file.is_open())
    {
        cout << "Couldn't open file " << endl;
        cout.flush();
        return (void *)-1;
    }
    while (temp->work)
    {
        sem_wait(temp->sem);
        for (int i = 0; i < 5; ++i)
        {
            cout << "2";
            cout.flush();
            out_file << "2";
            out_file.flush();
            sleep(1);
        }
        cout << endl;
        sem_post(temp->sem);
        sleep(1);
    }
    out_file.close();
    cout << "Поток 1 закончил работу" << endl;
    return 0;
}


int main()
{
    cout.setf(ios::unitbuf);
    pthread_t my_thread;
    p_args args;
    args.work = true;
    args.sem = sem_open("sem", O_CREAT, 0644, 1); 
    int status = 0;
    status = pthread_create(&my_thread, 0, some_p1, &args);
    if (!status)
        cout << "Поток 1 создан\n";
    else
        cout << "Ошибка создания потока " << status << endl;
    getch();
    args.work = false;
    pthread_join(my_thread, 0);
    cout << "Поток 1 успешно завершил работу" << endl;
    sem_close(args.sem);
    sem_unlink("sem");
    return 0;
}