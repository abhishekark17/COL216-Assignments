#include "main.hpp"
#include "CPU.hpp"
int main (int argc,char* argv[]) {

    CPU * cpu = new CPU (4, 100, "../INPUT", "../OUTPUT",10,2);
    cpu->run ();
    return 0;
}