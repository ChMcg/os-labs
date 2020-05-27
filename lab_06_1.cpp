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
    cout << "[1] Поток 1 начал работу\n";
    p_args *temp = (p_args *)arg;
    char *letters = (char*)shmat(temp->memory, NULL, 0644 | IPC_CREAT);
    char pointer = 'A';
    while (temp->work){
        if (pointer == 'D')
            pointer = 'A';
        letters[0] = pointer;
        cout << endl <<  "Записано " << pointer << endl;
        pointer++;
        sem_post(temp->semw);
        sem_wait(temp->semr);
        sleep(1);
    }
    sem_post(temp->semw);
    cout << "[1] поток 1 завершил работу\n";
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
        cout << "[1] Поток создан\n";
    else
        cout << "[1] ошибка создания потока: " << status << endl;
    getch();
    args.work = false;
    pthread_join(my_thread, 0);
    cout << "[1] Первый поток остановлен\n";
    sem_close(args.semw);
    sem_close(args.semr);
    sem_unlink("sem_w");
    sem_unlink("sem_r");
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}