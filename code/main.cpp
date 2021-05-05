#include "main.hpp"
#include "CPU.hpp"

// * inputFolder outputFolder maxClockCycles numberOfCores coreQueueLength rowAccessDelay colAccessDelay
int main (int argc,char **argv) {

    int numberOfCores = 4;
    int maxClockCycles = 200;
    string inputFolderPath = "../INPUT/tc1";
    string outputFolderPath = "../OUTPUT/tc1";
    int rowAccessDelay = 10;
    int colAccessDelay = 2;
    int coreQueueLength = 3;

    if (argc != 8) {
        cout << "There should be exactly 7 arguments in the following order: " << endl
            << "inputFolder outputFolder maxClockCycles numberOfCores coreQueueLength rowAccessDelay colAccessDelay" << endl
            << "TERMINATED" << endl;
        return -1;
    }
    else {
        inputFolderPath = argv[1];
        outputFolderPath = argv[2];
        maxClockCycles = stoi (argv[3]);
        numberOfCores = stoi(argv[4]);
        coreQueueLength = stoi (argv[5]);
        rowAccessDelay = stoi (argv[6]);
        colAccessDelay = stoi (argv[7]);
    }
    
    if (colAccessDelay < 2) {
        cout << "Column access delay must be atleast 2." << endl;
        return -2;
    }

    CPU * cpu = new CPU (numberOfCores, maxClockCycles,inputFolderPath, outputFolderPath,rowAccessDelay,colAccessDelay,coreQueueLength);
    cpu->run ();
    return 0;
}