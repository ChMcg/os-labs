#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

using namespace std;

struct funcargs{
    bool stop;
    char fname[25] = "./output.txt";
    sem_t *semw;
    sem_t *semr;
    int memdes = 0;
};

void *func1(void *arg){
    cout << "\nfunc1 started\n";
    funcargs *temp = (funcargs *)arg;
    char *letters = (char *)mmap(0, 1, PROT_WRITE, MAP_SHARED, temp->memdes, 0);
    char pointer = 'A';
    while (!(temp->stop)){
        if (pointer == 'D')
            pointer = 'A';
        letters[0] = pointer;
        cout << endl <<  "WROTE " << pointer << endl;
        pointer++;
        sem_post(temp->semw);
        sem_wait(temp->semr);
        sleep(1);
    }
    sem_post(temp->semw);
    cout << "\nfunc1 ended\n";
    return 0;
}

int main(){
    pthread_t f1, f2;
    funcargs args;
    args.stop = false;
    args.semw = sem_open("sem_w", O_CREAT, 0644, 0);
    args.semr = sem_open("sem_r", O_CREAT, 0644, 0); 
    int status = 0;
    int memorydes = shm_open("mem", O_CREAT | O_RDWR, 0644);
    if (memorydes == -1)
        perror("shm_open");
    else
    {
        args.memdes = memorydes;
        ftruncate(memorydes, 1);
    }

    status = pthread_create(&f1, 0, func1, &args);
    if (!status)
        cout << "thread1 created\n";
    else
        cout << "thread1 creation error " << status << endl;

    getchar();
    args.stop = true;
    pthread_join(f1, 0);
    cout << "\nfirst thread stoped\n";
    sem_close(args.semw);
    sem_close(args.semr);
    sem_unlink("sem_w");
    sem_unlink("sem_r");
    close(args.memdes);
    return 0;
}