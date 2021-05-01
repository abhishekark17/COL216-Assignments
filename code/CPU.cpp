#include "CPU.hpp"


CPU::CPU (int n,int m,  string fpi, string fpo,int rad,int cad, string outputFolderPath) { 
    numCores = n;
    folderPath = fpi;
    maxClockCycle = m;
    folderPathOutput = fpo;
    ROW_ACCESS_DELAY = rad;
    COL_ACCESS_DELAY = cad;
    
    outputHandler = new OutputHandler (numCores, outputFolderPath);
    allCores = new vector<CORE*>();
    dram = new DRAM (outputHandler);
    
    memoryRequestManager = new MRM (dram,allCores,numCores,ROW_ACCESS_DELAY, COL_ACCESS_DELAY, outputHandler,3);
    
    for (int i = 0; i < n; i++) {
        allCores->push_back(new CORE (folderPath + "/" +to_string(i+1)+".txt", i+1,folderPathOutput + "/" +to_string(i+1)+".txt",numCores,dram->getNumDRAMCols(),dram->getNumDRAMRows(), outputHandler,ROW_ACCESS_DELAY, COL_ACCESS_DELAY));
    }
    
    curClockCycle = 0;
    }

void CPU::exit () {
         cout << "cpu exited" <<endl;
         outputHandler->close();
     }

void CPU::run () {
    for (curClockCycle = 1; curClockCycle <= maxClockCycle; curClockCycle++) { 
        cout<<"Cycle: "<<curClockCycle<<"\t";
        memoryRequestManager->execute(allCores,curClockCycle);
        for (int i = 0; i < numCores; i++) { 
            if (allCores->at(i)->isRunnable()) allCores->at(i)->run(memoryRequestManager); 
            //allCores->at(i)->run(memoryRequestManager);   
        }
        outputHandler->printAll (curClockCycle);
    cout<<endl;
    }

    exit ();

}

int CPU::getNumCores() {return numCores;}
