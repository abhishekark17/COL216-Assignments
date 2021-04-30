#include "main.hpp"
#include "CPU.hpp"
int main (int argc,char* argv[]) {
    cout<<"in main"<<endl;
    CPU * cpu = new CPU (4, 5, "../INPUT", "../OUTPUT",10,2);
    cout<<"cpu initialised"<<endl;
    cpu->run ();
    return 0;
}