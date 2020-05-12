#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>

using namespace std;

struct p_args
{
    bool stop;
};

void *func1(void *arg){
    cout << "\nfunc1 started\n";
    p_args *temp = (p_args *)arg;
    struct stat file_status;
    int filedes = open("my_pipe", O_WRONLY);
    if (filedes == -1){
        perror("open");
        return (void *)-1;
    }
    int status;
    char letter[1];
    letter[0] = 'A';
    while (!(temp->stop)){

        if (letter[0] == 'D')
            letter[0] = 'A';

        status = write(filedes, letter, 1);
        if (status == -1)
            perror("write");
        else 
            cout << endl << "WROTE " << letter << endl;
        cout.flush();
        letter[0] += 1;

        sleep(1);
    }
    close(filedes);
    cout << "\nfunc1 ended\n";
    return 0;
}


int main(){
    pthread_t f1, f2;
    p_args args;
    args.stop = false;
    signal(SIGPIPE, SIG_IGN);
    int status = 0;
    status = mkfifo("my_pipe", S_IRUSR| S_IWUSR);
    if (status == -1)
        perror("mkfifo");
    status = pthread_create(&f1, 0, func1, &args);
    if (!status)
        cout << "thread1 created\n";
    else
        cout << "thread1 creation error " << status << endl;
    getchar();
    args.stop = true;
    pthread_join(f1, 0);
    cout << "\nfirst thread stoped\n";
    unlink("my_pipe");

    return 0;
}