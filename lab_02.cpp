#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <iostream>

#include "getch.cpp"
using namespace std;

void* some_p1(void * param);
void* some_p2(void * param);

struct p_args
{
    bool work;
    pthread_mutex_t* mutex;
};

int main(int argc, char* argv[])
{
    pthread_t my_thread_1, my_thread_2;
    pthread_attr_t my_attr_1; 
    pthread_attr_init(&my_attr_1);
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);
    p_args p1_arg = {true, &mutex};
    p_args p2_arg = {true, &mutex};
    pthread_create(&my_thread_1, &my_attr_1, some_p1, (void*)&p1_arg);        
    pthread_create(&my_thread_2, &my_attr_1, some_p2, (void*)&p2_arg);        
    getch();
    p1_arg.work = false;
    p2_arg.work = false;
    pthread_join(my_thread_1, nullptr);
    pthread_join(my_thread_2, nullptr);
    cout << endl;
    return 0;
}

void* some_p1(void* param)
{
    p_args* args = (p_args*)param;
    timespec tr = {0, 1000*1000*500};
    while(args->work)
    {
        if(pthread_mutex_lock(args->mutex) == 0)
        {
            for(size_t i = 0; i < 5; i++)
            {
                write(1, "1", sizeof("1"));
                // nanosleep(&tr, nullptr);
            }
        }
        pthread_mutex_unlock(args->mutex);
        sleep(1);
    }

    pthread_exit(0);
}

void* some_p2(void* param)
{
    p_args* args = (p_args*)param;
    timespec tr = {0, 1000*1000*500};
    while(args->work)
    {
        if(pthread_mutex_lock(args->mutex) == 0)
        {
            for(size_t i = 0; i < 5; i++)
            {
                write(1, "2", sizeof("2"));
                // nanosleep(&tr, nullptr);
            }
        }
        pthread_mutex_unlock(args->mutex);
        sleep(1);
    }

    pthread_exit(0);
}