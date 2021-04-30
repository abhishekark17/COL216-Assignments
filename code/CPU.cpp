#include "CPU.hpp"


CPU::CPU (int n,int m,  string fpi, string fpo,int rad,int cad) { 
    numCores = n;
    folderPath = fpi;
    maxClockCycle = m;
    folderPathOutput = fpo;
    ROW_ACCESS_DELAY = rad;
    COL_ACCESS_DELAY = cad;
    
    allCores = new vector<CORE*>();
    dram = new DRAM ();
    memoryRequestManager = new MRM (dram,allCores,numCores,ROW_ACCESS_DELAY, COL_ACCESS_DELAY, 20);
    
    for (int i = 0; i < n; i++) {
        allCores->push_back(new CORE (folderPath + "/" +to_string(i+1)+".txt", i+1,folderPathOutput + "/" +to_string(i+1)+".txt",numCores,dram->getNumDRAMCols(),dram->getNumDRAMRows()));
    }
    
    curClockCycle = 0;
    }

void CPU::exit () {
         cout << "cpu exited" <<endl;
     }

void CPU::run () {
    for (curClockCycle = 1; curClockCycle <= maxClockCycle; curClockCycle++) { 
        cout<<"Cycle: "<<curClockCycle;
        memoryRequestManager->execute(allCores,curClockCycle);
        for (int i = 0; i < numCores; i++) { 
            if (allCores->at(i)->isRunnable()) allCores->at(i)->run(memoryRequestManager);
            
        }
    cout<<endl;
    }

    exit ();

}

int CPU::getNumCores() {return numCores;}
