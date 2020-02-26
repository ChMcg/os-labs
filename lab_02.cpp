#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <iostream>
#include <string.h>

#include "getch.cpp"
using namespace std;

void* some_p1(void * param);
void* some_p2(void * param);

// #define LOCK
// #define TRY_LOCK
#define TIMED_LOCK

// #define LOCK_MUTEX_ON_START

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
    #ifdef LOCK_MUTEX_ON_START
        pthread_mutex_lock(&mutex);
    #endif // LOCK_MUTEX_ON_START
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
    timespec locktime = {2, 0};
    int my_error = 0;
    while(args->work)
    {
        #ifdef LOCK
            if(pthread_mutex_lock(args->mutex) == 0)
            {
                for(size_t i = 0; i < 5; i++)
                {
                    write(1, "1", sizeof("1"));
                    nanosleep(&tr, nullptr);
                }
                
                #ifdef TEST_MUTEX_LOCK
                    pthread_exit(0);
                #endif
            }
            pthread_mutex_unlock(args->mutex);
            sleep(1);
        #endif // LOCK
        #ifdef TRY_LOCK
            if((my_error = pthread_mutex_trylock(args->mutex)) == 0)
            {
                for(size_t i = 0; i < 5; i++)
                {
                    write(1, "1", sizeof("1"));
                    // nanosleep(&tr, nullptr);
                }
                write(1, "\n", sizeof("\n"));
                
                #ifdef TEST_MUTEX_LOCK
                    pthread_exit(0);
                #endif
                sleep(2);
                pthread_mutex_unlock(args->mutex);
            }
            else 
            {
                // sleep(1);
                // write(1, "\n[Поток 1]: mutex занят\n", sizeof("\n[Поток 1]: mutex занят\n"));
                write(1, "[Поток 1]: ", sizeof("[Поток 1]: "));
                // perror("[Поток 1]");
                write(1, strerror(my_error), strlen(strerror(my_error)));
                write(1, "\n", strlen("\n"));
                // sleep(1);
                // strerror(my_error);
            }
            sleep(1);
        #endif // TRY_LOCK
        #ifdef TIMED_LOCK
            clock_gettime(CLOCK_REALTIME, &locktime);
            locktime.tv_sec += 5;
            if(pthread_mutex_timedlock(args->mutex, &locktime) == 0)
            {
                for(size_t i = 0; i < 5; i++)
                {
                    write(1, "1", sizeof("1"));
                    // nanosleep(&tr, nullptr);
                }
                
                #ifdef TEST_MUTEX_LOCK
                    pthread_exit(0);
                #endif
            }
            else continue;
            pthread_mutex_unlock(args->mutex);
            sleep(1);        
        #endif // TIMED_LOCK
    }

    pthread_exit(0);
}

void* some_p2(void* param)
{
    p_args* args = (p_args*)param;
    timespec tr = {0, 1000*1000*500};
    timespec locktime = {2, 0};
    int my_error = 0;
    while(args->work)
    {
        #ifdef LOCK
            if(pthread_mutex_lock(args->mutex) == 0)
            {
                for(size_t i = 0; i < 5; i++)
                {
                    write(1, "2", sizeof("2"));
                    nanosleep(&tr, nullptr);
                }
                
                #ifdef TEST_MUTEX_LOCK
                    pthread_exit(0);
                #endif
            }
            pthread_mutex_unlock(args->mutex);
            sleep(1);
        #endif // LOCK
        #ifdef TRY_LOCK
            if((my_error = pthread_mutex_trylock(args->mutex)) == 0)
            {
                for(size_t i = 0; i < 5; i++)
                {
                    write(1, "2", sizeof("2"));
                    // nanosleep(&tr, nullptr);
                }
                write(1, "\n", sizeof("\n"));

                
                #ifdef TEST_MUTEX_LOCK
                    pthread_exit(0);
                #endif
                sleep(2);
                pthread_mutex_unlock(args->mutex);
            }
            else
            {
                // write(1, "\n[Поток 2]: mutex занят\n", sizeof("\n[Поток 2]: mutex занят\n"));
                write(1, "[Поток 2]: ", sizeof("[Поток 2]: "));
                // perror("[Поток 2]");
                write(1, strerror(my_error), strlen(strerror(my_error)));
                write(1, "\n", strlen("\n"));
                // sleep(1);
            }
            
            sleep(1);
        #endif // TRY_LOCK
        #ifdef TIMED_LOCK
            clock_gettime(CLOCK_REALTIME, &locktime);
            locktime.tv_sec += 5;
            if(pthread_mutex_timedlock(args->mutex, &locktime) == 0)
            {
                for(size_t i = 0; i < 5; i++)
                {
                    write(1, "2", sizeof("2"));
                    // nanosleep(&tr, nullptr);
                }
                
                #ifdef TEST_MUTEX_LOCK
                    pthread_exit(0);
                #endif
            }
            else continue;
            pthread_mutex_unlock(args->mutex);
            sleep(1);
        #endif // TIMED_LOCK
    }

    pthread_exit(0);
}