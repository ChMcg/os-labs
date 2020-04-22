#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>

using namespace std;

struct funcargs{
    bool stop;
    char fname[25] = "./output.txt";
    sem_t *sem;
};

void *func1(void *arg){
    cout << "\nfunc1 in laba6 started working\n";
    funcargs *temp = (funcargs *)arg;
    ofstream out_file;
    out_file.open(temp->fname, ios::app);
    if (!out_file.is_open()){
        cout << "Couldn't open file " << endl;
        cout.flush();
        return (void *)-1;
    }
    while (!(temp->stop)){
        sem_wait(temp->sem);
        for (int i = 0; i < 10; ++i){
            cout << "1";
            cout.flush();
            out_file << "1";
            out_file.flush();
            sleep(1);
        }
        sem_post(temp->sem);
        sleep(1);
    }
    out_file.close();
    cout << "\nfunc1 in laba6 ended working\n";
    return 0;
}

void *func2(void *arg){
    cout << "\nfunc2 started working\n";
    funcargs *temp = (funcargs *)arg;
    while (!(temp->stop)){

    }
    cout << "\nfunc2 ended working\n";
    return 0;
}

int main(){
    pthread_t f1, f2;
    funcargs args;
    args.stop = false;
    // sem_unlink("./semafor");
    args.sem = sem_open("sem", O_CREAT, 0644, 1); // NONBLOCK не нужен?
    int status = 0;
    
    status = pthread_create(&f1, 0, func1, &args);
    if (!status)
        cout << "thread1 created\n";
    else
        cout << "thread1 creation error " << status << endl;
    

    getchar();
    args.stop = true;
    pthread_join(f1, 0);
    cout << "\nfirst thread stoped\n";
    sem_close(args.sem);
    sem_unlink("sem");
    return 0;
}