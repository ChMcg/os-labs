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
    int data = 0;
    while (temp->work)
    {
        if (data == 4)
            data = 0;
        while ((mq_send(temp->mqdes, (char *)&data, sizeof(data), data) == -1) && (temp->work))
        {
            perror("mq_send");
            sleep(1);
        }
        if (temp->work)
            cout << endl << "Записано " << data << " с приоритетом " << data << endl;

        data += 1;
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
        mq_getattr(status, &attr);
        cout << endl << "размер сообщения = " << attr.mq_msgsize << endl;
        cout << endl << "максимальный размер сообщения = " << attr.mq_maxmsg << endl;
        cout << endl << "mq_flags = " << attr.mq_flags<< endl;
        cout << endl << "mq_curmsg = " << attr.mq_curmsgs << endl;
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
    mq_unlink(args.mqname);
    return 0;
}