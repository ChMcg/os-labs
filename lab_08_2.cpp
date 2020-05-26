#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fcntl.h>
#include <mqueue.h>
#include "getch.cpp"

using namespace std;

struct p_args
{
    bool work;
    char mqname[20] = "/test_mq";
    mqd_t mqdes = 0;
};

void *some_p1(void *arg)
{
    cout << "Поток 1 начал работу" << endl;
    p_args *temp = (p_args *)arg;
    int status;
    int mess = 999;
    while (temp->work)
    {
        unsigned int priority = 666;
        while ((mq_receive(temp->mqdes, (char*)&mess, sizeof(mess), &priority) == -1) && (temp->work))
        {
            perror("mq_send");
            sleep(1);
        }
        if (temp->work)
            cout << endl << "READ " << mess << " WITH PRIORITY " << priority << endl;
        sleep(1);
    }
    cout << "Поток 1 закончил работу" << endl;
    return 0;
}


int main()
{
    pthread_t my_thread;
    p_args args;
    args.work = true;
    int status = 0;
    mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 3;
    attr.mq_msgsize = 4;
    attr.mq_curmsgs = 0;
    status = mq_open(args.mqname, O_CREAT | O_RDWR | O_NONBLOCK, 0644, &attr);
    if (status == -1)
        perror("mq_open");
    else 
    {
        args.mqdes = status;
        status = pthread_create(&my_thread, 0, some_p1, &args);
        if (!status)
            cout << "Поток 1 создан\n";
        else
            cout << "Ошибка создания потока " << status << endl;
    }
    getch();
    args.work = false;
    pthread_join(my_thread, 0);
    cout << "Поток 1 завершил работу" << endl;
    mq_close(args.mqdes);
    return 0;
}