#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <queue>
#include <sys/utsname.h>

#include "config.cpp"
#include "getch.cpp"


using namespace std;

struct p_args
{
    int server_socket_des;
    int client_socket_des;
    queue<sending_data> proccess_queue;
    queue<sending_data> send_queue;
    bool stop_threads = false;
    pthread_mutex_t proccess_queue_mutex;
    pthread_mutex_t send_queue_mutex;
    struct sockaddr_in address;
};

pthread_t waiting_thread, receive_thread, proccess_thread, send_thread;

void* recieve_data(void* arg){
    // cout << "Function recieve_data started working" << endl;
    // cout << "Function recieve_data started working" << endl;
    p_args* info = (p_args*)arg;
    sending_data temp;
    while(!(info->stop_threads)){
        // if (!(check(recv(info->client_socket_des, &temp, sizeof(temp), 0), "Receiving error"))){
        memset(&temp, 0, sizeof(temp));
        // memset(&info->address, 0, sizeof(info->address));
        int len = sizeof(info->address);
        if (!(check(recvfrom(info->server_socket_des, &temp, sizeof(temp), 0, (sockaddr *)&info->address, (socklen_t *)&len), "Receiving error"))){
            cout << "Received data with messege \"" << temp.msg << "\"" << endl;
            pthread_mutex_lock(&info->proccess_queue_mutex);
            info->proccess_queue.push(temp);
            pthread_mutex_unlock(&info->proccess_queue_mutex);
        }
        sleep(1);
    }
    // cout << "Function recieve_data stopped working" << endl;
    return 0;
}

void* process_data(void* arg){
    // cout << "Function process_data started working" << endl;
    p_args* info = (p_args*)arg;
    sending_data temp;
    while(!(info->stop_threads)){
        pthread_mutex_lock(&info->proccess_queue_mutex);
        if (info->proccess_queue.size() == 0){
            pthread_mutex_unlock(&info->proccess_queue_mutex);
            sleep(1);
            continue;
        }
        temp = info->proccess_queue.front();
        temp.info = sysconf(_SC_STREAM_MAX);
        strncpy(temp.msg, "Processed", 9);
        cout << "Data processed" << endl;
        pthread_mutex_lock(&info->send_queue_mutex);
        info->send_queue.push(temp);
        pthread_mutex_unlock(&info->send_queue_mutex);
        info->proccess_queue.pop();
        pthread_mutex_unlock(&info->proccess_queue_mutex);
    }
    // cout << "Function process_data stopped working" << endl;
    return 0;
}

void* send_data(void* arg){
    // cout << "Function send_data started working" << endl;
    p_args* info = (p_args*)arg;
    while(!(info->stop_threads)){
        pthread_mutex_lock(&info->send_queue_mutex);
        if (info->send_queue.size() == 0){
            pthread_mutex_unlock(&info->send_queue_mutex);
            continue;
        }
        if (!(check(sendto(info->server_socket_des, &info->send_queue.front(), sizeof(sending_data), 0, (sockaddr *)&info->address, sizeof(info->address)), "Sending error"))){
            cout << "Response sent successfuly" << endl;
            info->send_queue.pop();
        }
        pthread_mutex_unlock(&info->send_queue_mutex);
        sleep(1);
    }
    // cout << "Function send_data stopped working" << endl;
    return 0;
}

void* wait_connections(void* arg){
    // cout << "Function wait_connections started working" << endl;
    p_args* info = (p_args*)arg;
    int temp_status;
    int adrlen = sizeof(info->address);
    while(!(info->stop_threads)){

        if (!(check(pthread_create(&receive_thread, 0, recieve_data, arg), "Receive pthread_create")) &&
               !(check(pthread_create(&proccess_thread, 0, process_data, arg), "Proccess pthread_create")) &&
               !(check(pthread_create(&send_thread, 0, send_data, arg), "Send pthread_create")))
            cout << "Creation success" << endl;
        break;

        sleep(1);
    }
    // cout << "Function wait_connections stopped working" << endl;
    return 0;
}

int main(){
    cout.setf(ios::unitbuf);
    int temp_status;
    p_args args;

    check(pthread_mutex_init(&args.proccess_queue_mutex, 0), "Mutex init");
    check(pthread_mutex_init(&args.send_queue_mutex, 0), "Mutex init");

    if (!(check(temp_status = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0), "Socket opening"))){
        args.server_socket_des = temp_status;

        args.address.sin_family = AF_INET; 
        args.address.sin_addr.s_addr = INADDR_ANY; 
        args.address.sin_port = htons( 8000 ); 
        // args.address.sin_port = htons( 8880 ); 
        if (!(check(bind(args.server_socket_des, (struct sockaddr *)&args.address,  sizeof(args.address)), "Bind failed"))) { 
            cout << "Binded successfuly" << endl;
        } 
    }

    if (!(check(pthread_create(&waiting_thread, 0, wait_connections, &args), "Waiting thread pthread_create")))
        cout << "Waiting thread created" << endl;
    
    getch();
    args.stop_threads = true;
    pthread_join(waiting_thread, 0);
    pthread_join(receive_thread, 0);
    pthread_join(proccess_thread, 0);
    pthread_join(send_thread, 0);
    close(args.server_socket_des);
    perror("\nServer socket closing");
    return 0;
}