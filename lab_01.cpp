#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <time.h>

#include "getch.cpp"
using namespace std;

void* some_p1(void * param);
void* some_p2(void * param);

int main(int argc, char* argv[])
{
    pthread_t my_thread_1, my_thread_2;
    bool p1_work = true;
    bool p2_work = true;
    pthread_attr_t my_attr_1; 
    pthread_attr_init(&my_attr_1);
    if(pthread_create(&my_thread_1, &my_attr_1, some_p1, (void*)&p1_work) == 0)
    {
        cout << "Поток 1 начал работу" << endl;
    }      
    if(pthread_create(&my_thread_2, &my_attr_1, some_p2, (void*)&p2_work) == 0)
    {
        cout << "Поток 2 начал работу" << endl;
    }
    
    getch();
    p1_work = false;
    p2_work = false;

    void* status_1, *status_2;
    pthread_join(my_thread_1, &status_1);
    pthread_join(my_thread_2, &status_2);
    cout << endl;
    cout << "Поток-1 вернул: " << *(int*)status_1 << endl;
    cout << "Поток-2 вернул: " << *(int*)status_2 << endl;
    free((int*)status_1);
    free((int*)status_2);
    return 0;
}

void* some_p1(void* param)
{
    sleep(1);
    int cnt = 0;
    timespec tr = {0, 1000*1000*250};
    while(*(bool*)param)
    {
        // cout << "1";
        // printf("1");
        write(1, "1", sizeof("1"));
        cnt++;
        // sleep(1);
        nanosleep(&tr, nullptr);
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
    while(*(bool*)param)
    {
        // cout << "2";
        // printf("2");
        write(1, "2", sizeof("2"));
        cnt++;
        // sleep(1);
        nanosleep(&tr, nullptr);
    }

    int* ret_status = new int;
    *ret_status = cnt;
    pthread_exit((void*)ret_status);
}