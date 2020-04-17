#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

using namespace std;

int main(int len, char **args){
    cout << "Programm 2 started working\n";
    cout << "Current proccess id : " << getpid() << endl;
    cout << "Parent proccess id : " << getppid() << endl;
    pid_t pid = fork();
    if (pid == 0){
        // child
        if (execl("./child", "1", "2", "3", NULL) == -1)
            perror("Execl error : ");
    }
    else{
        //parent
        cout << "Programm 2 waiting for Programm 1...." << endl;
        int res;
        waitpid(pid, &res, 0);
        cout << "Programm 1 returned code " << WEXITSTATUS(res) << endl;
    }
    cout << "Programm 2 ended working" << endl;
    return 0;
}