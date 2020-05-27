#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;

int main(int len, char **args){
    cout.setf(ios::unitbuf);
    string pn = "[1] ";
    cout << pn << "Программа 1 начала работу" << endl;
    cout << pn << "ID процесса " << getpid() << endl;
    cout << pn << "ID родительского процесса: " << getppid() << endl;
    int res = 0;
    for(int i = 0; i < len; ++i)
    {
        cout << pn << "аргумент №" << i + 1 << " " << args[i] << endl;
        res++;
        sleep(1);
    }
    cout << pn << "Программа 1 завершила работу с кодом " << res << endl;
    return res;
}