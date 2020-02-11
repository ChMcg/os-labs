#include <pthread.h>
#include <unistd.h>
#include <iostream>
using namespace std;
void* some_func(void * some);

int main(int argc, char* argv[])
{
    pthread_t my_thread;
    pthread_attr_t my_attr; 
    pthread_attr_init(&my_attr);
    pthread_create(&my_thread, &my_attr, some_func, argv[1]);
	for(int i = 0; i < 5; i++)
	{
		sleep(2);
		cout << "1";
	}
		
	
    pthread_join(my_thread, nullptr);
	cout << endl;
    return 0;
}

void* some_func(void * some)
{
    for(int i = 0; i<10; i++)
	{
		sleep(1);
		cout << "2";
	}
    pthread_exit(0);
}
