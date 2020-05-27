#include "getch.cpp"

#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>


using namespace std;

struct p_args
{
    bool work;
    sem_t *semw;
    sem_t *semr;
    int memory = 0;
};

void *some_p1(void *arg)
{
    cout << "[2] Поток 1 начал работу\n";
    p_args *temp = (p_args *)arg;
    char *letters = (char*)shmat(temp->memory, NULL, 0644 | IPC_CREAT);
    while (temp->work)
    {
        char t;
        sem_wait(temp->semw);
        t = letters[0];
        cout << endl << "Прочитано: " << t << endl;
        sem_post(temp->semr);
        sleep(1);
    }
    sem_post(temp->semr);
    cout << "[2] поток 1 завершил работу\n";
    return 0;
}


int main()
{
    pthread_t my_thread;
    p_args args;
    args.work = true;
    args.semw = sem_open("sem_w", O_CREAT, 0644, 0);
    args.semr = sem_open("sem_r", O_CREAT, 0644, 0); 
    int status = 0;
    int shmid = shmget(ftok("mem", 0), 1024, 0644 | IPC_CREAT);
    if (shmid == -1)
        perror("shmat");
    else
    {
        args.memory = shmid;
    }
    status = pthread_create(&my_thread, 0, some_p1, &args);
    if (!status)
        cout << "[2] Поток создан\n";
    else
        cout << "[2] ошибка создания потока: " << status << endl;
    getch();
    args.work = false;
    pthread_join(my_thread, 0);
    cout << "[2] Первый поток остановлен\n";
    sem_close(args.semw);
    sem_close(args.semr);
    sem_unlink("sem_w");
    sem_unlink("sem_r");
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}