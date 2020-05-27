#include <unistd.h>
#include <iostream>
#include <string>
#include <sched.h>
#include <sys/wait.h>

using namespace std;

int some_p1(void * param);

int main(int len, char **args){
    cout.setf(ios::unitbuf);
    string pn = "[3] ";
    cout << pn << "Программа 2 начала работу" << endl;
    cout << pn << "ID процесса " << getpid() << endl;
    cout << pn << "ID родительского процесса: " << getppid() << endl;
    void* stack = malloc(1024);
    pid_t pid = clone(&some_p1, stack, SIGCHLD, NULL);
    int res = 0;
    cout << pn << "Ожидание завершения программы 1" << endl;
    pid_t ret = waitpid(pid, &res, 0);
    if (ret == -1) 
        perror("waitpid");
    cout << pn << "Программа 1 вернула код " << WEXITSTATUS(res) << endl;
    cout << pn << "Программа 3 завершила работу" << endl;
    return 0;
}


int some_p1(void * param)
{
    string pn = "[3] ";
    cout << pn << "Поток 1 запущен (pid: " << getpid() << ")" << endl;
    if (execle("./build/lab_04_1", "1", "2", "3", NULL, NULL) == -1)
        perror("execle error");
    cout << pn << "Поток 1 завершён" << endl;
    return 0;
}
