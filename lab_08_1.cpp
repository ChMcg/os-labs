#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "getch.cpp"

using namespace std;

struct p_args
{
    bool work;
    int msgid;
};

struct TMessage {
    long mtype;
    char buff[256];
};

void *some_p1(void *arg)
{
    cout << "Поток 1 начал работу" << endl;
    p_args *temp = (p_args *)arg;
    int status;
    int data = 0;
    int len;
    int msgid = temp->msgid;
    while (temp->work)
    {
        if (data == 4)
            data = 0;
        TMessage msg;
        msg.mtype = 1;
        int len = sprintf(msg.buff,"hello, %d", data);
        status = msgsnd(msgid, &msg, len, IPC_NOWAIT);
        if (status < 0)
        {
            cout << "Сообщение не было отправлено" << endl;
        }
        else
        {
            cout << "Отправлено: " << data << endl;
        }
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
    key_t key = ftok("lab_8",'A');
    int msgid = msgget(key, 0);
    if (msgid < 0)
    {
        msgid = msgget(key,0666 | IPC_CREAT);
    }
    msginfo test;
    test.msgtql = 0;
    msgctl(msgid, IPC_INFO, (msqid_ds*)&test);
    cout << test.msgtql << endl;
    // cout << test.msg_qbytes << endl;
    args.msgid = msgid;
    status = pthread_create(&my_thread, 0, some_p1, &args);
    if (!status)
        cout << "Поток 1 создан\n";
    else
        cout << "Ошибка создания потока " << status << endl;
    
    getch();
    args.work = false;
    pthread_join(my_thread, 0);
    cout << "Поток 1 завершил работу" << endl;
    // msgctl(msgid, IPC_RMID, NULL);
    return 0;
}