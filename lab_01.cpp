#include <pthread.h>
#include <iostream>
using namespace std;
void* some_func(void * some);

int main(int argc, char* argv[])
{
    pthread_t my_thread;
    pthread_attr_t my_attr; 
    pthread_attr_init(&my_attr);
    pthread_create(&my_thread, &my_attr, some_func, argv[1]);

    pthread_join(my_thread, nullptr);
    return 0;
}

void* some_func(void * some)
{
    cout << "hi there" << endl;
    pthread_exit(0);
}
