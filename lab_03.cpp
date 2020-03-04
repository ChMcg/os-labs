#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <time.h>

#include "getch.cpp"
using namespace std;

const size_t BUFF_SIZE = 128;

void* some_p1(void * param);
void* some_p2(void * param);

struct p_args
{
    bool work;
    int* fd[2];
};

int main(int argc, char* argv[])
{
    pthread_t my_thread_1, my_thread_2;
    pthread_attr_t my_attr_1; 
    pthread_attr_init(&my_attr_1);
    
    int pipe_fd[2];
    pipe(pipe_fd);
    // int flag = fcntl(pipe_fd[0], F_GETFL, 0);
    fcntl(pipe_fd[0], F_SETFL, 
        fcntl(pipe_fd[0], F_GETFL, 0) | O_NONBLOCK);
    // flag = fcntl(pipe_fd[1], F_GETFL, 0);
    fcntl(pipe_fd[1], F_SETFL, 
        fcntl(pipe_fd[1], F_GETFL, 0) | O_NONBLOCK);

    p_args p1_args = {true, pipe_fd};
    p_args p2_args = {true, pipe_fd};

    
    if(pthread_create(&my_thread_1, &my_attr_1, some_p1, (void*)&p1_args) == 0)
    {
        cout << "Поток 1 начал работу" << endl;
    }      
    if(pthread_create(&my_thread_2, &my_attr_1, some_p2, (void*)&p2_args) == 0)
    {
        cout << "Поток 2 начал работу" << endl;
    }
    
    getch();
    p1_args.work = false;
    p2_args.work = false;

    void* status_1, *status_2;
    pthread_join(my_thread_1, &status_1);
    pthread_join(my_thread_2, &status_2);
    cout << endl;
    cout << "Поток-1 вернул: " << *(int*)status_1 << endl;
    cout << "Поток-2 вернул: " << *(int*)status_2 << endl;
    return 0;
}

void* some_p1(void* param)
{
    sleep(1);
    int cnt = 0;
    p_args* args = (p_args*)param;
    timespec tr = {0, 1000*1000*250};
    char ch = 'a';
    while(args->work)
    {
        // if(write(*(*(args->fd)+1), &ch, 1) == -1)
        if(write(*(*(args->fd)+1), nullptr, 0) <= 0)
            perror("[Поток 1]");
        else
            // printf("\n[Поток 1]-[запись]: %c\n", ch);
            cout << "[Поток 1]-[запись]: " << ch << endl;
            cout.flush();
        if (ch < 'Z')
            ch++;
        else
        {
            ch = 'A';
        }        
        // sleep(1);
        usleep(500*1000);

        cnt++;
    }

    int* ret_status = new int;
    *ret_status = cnt;
    pthread_exit((void*)ret_status);
}

void* some_p2(void* param)
{
    sleep(1);
    int cnt = 0;
    timespec tr = {0, 1000*1000*500};
    p_args* args = (p_args*)param;
    while(args->work)
    {
        char ch;
        if(read(*(*(args->fd)+0), &ch, 1) == -1)
            perror("[Поток 2]");
        else 
            cout << "[Поток 2]-[Чтение]: " << ch << endl;
        cnt++;
        usleep(500*1000);
    }

    int* ret_status = new int;
    *ret_status = cnt;
    pthread_exit((void*)ret_status);
}