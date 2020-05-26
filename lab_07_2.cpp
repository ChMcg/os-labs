#include "getch.cpp"

#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>


using namespace std;

struct p_args
{
    bool work;
    sem_t *semw;
    sem_t *semr;
    int memdes = 0;
};

void *func1(void *arg)
{
    cout << "[2] поток 1 начал работу\n";
    p_args *temp = (p_args *)arg;
    char *letters = (char *)mmap(0, 1, PROT_READ, MAP_SHARED, temp->memdes, 0);
    while (temp->work)
    {
        char t;
        sem_wait(temp->semw);
        t = letters[0];
        cout << endl << "Прочитано : " << t << endl;
        sem_post(temp->semr);
        sleep(1);
    }
    sem_post(temp->semr);
    cout << "[2] поток 1 завершил работу\n";
    return 0;
}


int main()
{
    pthread_t f1;
    p_args args;
    args.work = true;
    args.semw = sem_open("sem_w", O_CREAT, 0644, 0); 
    args.semr = sem_open("sem_r", O_CREAT, 0644, 0); 
    int status = 0;
    int memorydes = shm_open("mem", O_CREAT | O_RDWR, 0644);
    if (memorydes == -1)
        perror("shm_open : ");
    else
    {
        args.memdes = memorydes;
    }
    status = pthread_create(&f1, 0, func1, &args);
    if (!status)
        cout << "[2] Поток 1 создан\n";
    else
        cout << "[2] Ошибка создания потока: " << status << endl;
    getch();
    args.work = false;
    pthread_join(f1, 0);
    cout << "[2] Первый поток остановлен\n";
    sem_close(args.semw);
    sem_close(args.semr);
    sem_unlink("sem_w");
    sem_unlink("sem_r");
    return 0;
}