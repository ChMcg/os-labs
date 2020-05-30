#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <sys/utsname.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <signal.h>

#include "config.cpp"
#include "getch.cpp"


using namespace std;

struct p_args
{
    int server_socket_des;
    bool stop_threads = false;
    struct sockaddr_in serv_addr; 
};

pthread_t sending_thread, receiving_thread, connecting_thread;

void* send_request(void* arg)
{
    // my_cout{} << "Function send_request started working" << endl;
    p_args* info = (p_args*)arg;
    int temp_status;
    while(!(info->stop_threads))
    {
        struct sending_data temp;
        strncpy(temp.msg, "Sent", 5);
        if (!(check(sendto(info->server_socket_des, &temp, sizeof(temp), 0, (sockaddr *)&info->serv_addr, sizeof(info->serv_addr)), "Sending error")))
            my_cout{} << "Sent to server with message \"Sent\"" << endl;
        sleep(1);
    }
    // my_cout{} << "Function send_request stopped working" << endl;
    return 0;
}

void* get_response(void* arg)
{
    // my_cout{} << "Function get_response started working" << endl;
    p_args* info = (p_args*)arg;
    int temp_status;
    struct sending_data temp;
    while(!(info->stop_threads))
    {
        sleep(1);
        int lent = sizeof(info->serv_addr);
        if (!(check(recvfrom(info->server_socket_des, &temp, sizeof(temp), 0, (sockaddr *)&info->serv_addr, (socklen_t *)&lent), "Receiving error")))
        {
            my_cout{} << "Received data with messege \"" << temp.msg << "\"" << endl;
            my_cout{} << "Sysconf info: " << endl;
            my_cout{} << "_SC_STREAM_MAX: " << temp.info << endl;
        }
    }
    // my_cout{} << "Function get_response stopped working" << endl;
    return 0;
}

void* establish_connection(void* arg)
{
    // my_cout{} << "Function establish_connection started working" << endl;
    p_args* info = (p_args*)arg;
    int temp_status;
    struct sockaddr server_addr;

    while(!(info->stop_threads))
    { 
        memset(&info->serv_addr, '0', sizeof(info->serv_addr)); 
        info->serv_addr.sin_family = AF_INET; 
        // info->serv_addr.sin_port = htons(8880); 
        info->serv_addr.sin_port = htons(8000); 

        check(inet_pton(AF_INET, "127.0.0.1", &info->serv_addr.sin_addr), "Inet_pton");

        my_cout{} << "Creating working threads for this connection" << endl;
        if (!(check(pthread_create(&sending_thread, 0, send_request, arg), "Sending pthread_create")) &&
               !(check(pthread_create(&receiving_thread, 0, get_response, arg), "Geting pthread_create")));
        {
            my_cout{} << "Creation success" << endl;
            break;   
        }

        sleep(1);
    }
    // my_cout{} << "Function establish_connection stopped working" << endl;
    return 0;
}

int main()
{
    my_cout{}.setf(ios::unitbuf);
    int temp_status;
    p_args arg;

    signal(SIGPIPE, SIG_IGN);

    if (!check(temp_status = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0), "Socket opening"))
        arg.server_socket_des = temp_status;

    check(pthread_create(&connecting_thread, 0, establish_connection, &arg), "Connecting pthread_create");
    
    getch();
    arg.stop_threads = true;
    pthread_join(connecting_thread, 0);
    pthread_join(receiving_thread, 0);
    pthread_join(sending_thread, 0);
    close(arg.server_socket_des);
    perror("\nSocket closing");
    return 0;
}