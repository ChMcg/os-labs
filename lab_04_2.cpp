#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/wait.h>

using namespace std;

int main(int len, char **args){
    cout.setf(ios::unitbuf);
    string pn = "[2] ";
    cout << pn << "Программа 2 начала работу" << endl;
    cout << pn << "ID процесса " << getpid() << endl;
    cout << pn << "ID родительского процесса: " << getppid() << endl;
    pid_t pid = fork();
    if (pid == 0)
    {
        // child
        if (execle("./build/lab_04_1", "1", "2", "3", NULL, NULL) == -1)
            perror("execle error");
    }
    else
    {
        // parent
        cout << pn << "Ожидание завершения программы 1" << endl;
        int res;
        waitpid(pid, &res, 0);
        cout << pn << "Программма 1 вернула код " << WEXITSTATUS(res) << endl;
    }
    cout << pn << "Программа 2 завершила работу " << endl;
    return 0;
}