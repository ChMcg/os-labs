#include <unistd.h>
#include <pthread.h>
#include <iostream>

using namespace std;

class MyBuf{
private:
    char *buffer;
    int size;
    int in;
    int out;
    pthread_mutex_t mutex;
    pthread_cond_t cvw;
    pthread_cond_t cvr;
public:
    MyBuf(int size);
    ~MyBuf();
    void write(char);
    void read(char *);
};

MyBuf::MyBuf(int size){
    this->size = size;
    this->buffer = new char[size];
    pthread_mutex_init(&this->mutex, 0);
    pthread_cond_init(&this->cvw, 0);
    pthread_cond_init(&this->cvr, 0);
    this->in = 0;
    this->out = 0;
}

MyBuf::~MyBuf(){
    delete this->buffer;
}

void MyBuf::write(char ch){
    //printf("\nEntered write func\n");
    pthread_mutex_lock(&this->mutex);
    if (this->size == this->in)
        this->in = 0;
    if (this->in == this->out - 1 /*&& this->in != 0*/){
        printf("\nFunc write is waiting for signal\n");
        pthread_cond_wait(&this->cvw, &this->mutex);
    }
    this->buffer[in] = ch;
    this->in += 1;
    //pthread_cond_signal(&this->cvw);
    pthread_cond_broadcast(&this->cvr);
    pthread_mutex_unlock(&this->mutex);
    //printf("\nExited write func\n");
}

void MyBuf::read(char *ch){
    pthread_mutex_lock(&this->mutex);
    if (this->out == this->in){
        printf("\nFunc read is waiting for signal\n");
        pthread_cond_wait(&this->cvr, &this->mutex);
    }
    if (this->size == this->out)
        this->out = 0;
    *ch = this->buffer[out];
    this->out++;
    //pthread_cond_signal(&this->cvw);
    pthread_cond_broadcast(&this->cvw);
    pthread_mutex_unlock(&this->mutex);
}

struct funcargs{
    bool stop;
    MyBuf *buf;
};

void *func1(void *arg){
    cout << "\nfunc1 started working\n";
    funcargs *temp = (funcargs *)arg;
    char c = 'A';
    while (!(temp->stop)){
        if (c == 'Z' + 1)
            c = 'A';
        temp->buf->write(c);
        printf("WROTE %c\n", c);
        c++;
        sleep(1);
    }
    cout << "\nfunc1 ended working\n";
    return 0;
}

void *func2(void *arg){
    cout << "\nfunc2 started working\n";
    funcargs *temp = (funcargs *)arg;
    char c;
    while (!(temp->stop)){
        temp->buf->read(&c);
        printf("READ %c\n", c);
        sleep(1);
    }
    cout << "\nfunc2 ended working\n";
    return 0;
}


int main(){
    pthread_t f1, f2;
    funcargs args;
    args.stop = false;
    args.buf = new MyBuf(10);
    int status = 0;

    
    status = pthread_create(&f1, 0, func1, &args);
    if (!status)
        cout << "thread1 created\n";
    else
        cout << "thread1 creation error " << status << endl;
    
    status = pthread_create(&f2, 0, func2, &args);
    if (!status)
        cout << "thread2 created\n";
    else
        cout << "thread2 creation error " << status << endl;

    getchar();
    args.stop = true;
    pthread_join(f1, 0);
    cout << "\nfirst thread stoped\n";
    pthread_join(f2, 0);
    cout << "\nsecond thread stoped\n";
    return 0;
}