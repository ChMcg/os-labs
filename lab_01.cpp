#include <pthread.h>
#include <unistd.h>
#include <iostream>

#include "getch.cpp"
using namespace std;

void* some_p1(void * param);
void* some_p2(void * param);

int main(int argc, char* argv[])
{
    pthread_t my_thread_1, my_thread_2;
    bool p1_work = true;
    bool p2_work = true;
    pthread_attr_t my_attr_1, my_attr_2; 
    pthread_attr_init(&my_attr_1);
    pthread_attr_init(&my_attr_2);
    pthread_create(&my_thread_1, &my_attr_1, some_p1, (void*)&p1_work);        
    pthread_create(&my_thread_2, &my_attr_2, some_p2, (void*)&p2_work);        
    
    getch();
    p1_work = false;
    p2_work = false;

    pthread_join(my_thread_1, nullptr);
    pthread_join(my_thread_2, nullptr);
    cout << endl;
    return 0;
}

void* some_p1(void* param)
{
    while(*(bool*)param)
    {
        cout << "1";
        sleep(1);
    }

    pthread_exit(0);
}

void* some_p2(void* param)
{
    while(*(bool*)param)
    {
        cout << "2";
        sleep(2);
    }

    pthread_exit(0);
}