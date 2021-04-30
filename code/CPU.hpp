#include "main.hpp"
#include "Core.hpp"
#include "DRAM.hpp"
#include "MRM.hpp"

class CPU {
private:
    static int numCores;
    int curClockCycle;
    int maxClockCycle;
    vector<CORE*> * allCores;
    string folderPath;
    string folderPathOutput;
    int ROW_ACCESS_DELAY;
    int COL_ACCESS_DELAY;

    DRAM *dram;
    MRM *memoryRequestManager;
public: 
    CPU (int n,int m,  string fpi, string fpo,int rad,int cad) { 
        numCores = n;
        folderPath = fpi;
        maxClockCycle = m;
        folderPathOutput = fpo;
        ROW_ACCESS_DELAY = rad;
        COL_ACCESS_DELAY = cad;
        
        CORE::initAllCores();
        dram = new DRAM ();
        memoryRequestManager = new MRM (dram,allCores,numCores,ROW_ACCESS_DELAY, COL_ACCESS_DELAY, 20);
        
        for (int i = 0; i < n; i++) {
            allCores->push_back(new CORE (folderPath + "/" +to_string(i+1)+".txt", i+1,folderPathOutput + "/" +to_string(i+1)+".txt",numCores,dram->getNumDRAMCols(),dram->getNumDRAMRows()));
        }

        
        

        curClockCycle = 0;
     }

     void exit () {
         cout << "cpu exited" <<endl;
     }

     void run () {
         for (curClockCycle = 1; curClockCycle <= maxClockCycle; curClockCycle++) { 
             cout<<"Cycle: "<<curClockCycle;
            memoryRequestManager->execute(allCores,curClockCycle);
             for (int i = 0; i < numCores; i++) { 
                allCores->at(i)->run(memoryRequestManager);
                
            }
            cout<<endl;
         }

         exit ();

     }

     int getNumCores() {return numCores;}
};