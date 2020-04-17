#include <unistd.h>
#include <iostream>


using namespace std;

int main(int len, char **args){
    cout << "Programm 1 started working\n";
    cout << "Current proccess id : " << getpid() << endl;
    cout << "Parent proccess id : " << getppid() << endl;
    for(int i = 0; i < len; ++i){
        cout << "arg #" << i + 1 << " " << args[i] << endl;
        sleep(1);
    }
    int res = 5;
    cout << "Programm 1 ended working with code " << res << endl;
    return res;
}