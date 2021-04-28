#include "main.hpp"
#include "Core.hpp"
#include "DRAM.hpp"
#include "MRM.hpp"

class CPU {
private:
    int numCores, curClockCycle;
    int maxClockCycle;
    vector<CORE*> allCores;
    string folderPath;

    DRAM *dram;
    MRM *memoryRequestManager;
public: 
    CPU (int n,int m,  string fp) { 
        numCores = n;
        folderPath = fp;
        maxClockCycle = m;
        
        CORE::initAllCores();
        
        for (int i = 0; i < n; i++) {
            allCores.push_back(new CORE (folderPath + "/" +to_string(i+1)+".txt", i+1));
        }

        dram = new DRAM ();
        memoryRequestManager = new MRM ();
        

        curClockCycle = 0;
     }

     void exit ();

     void run () {
         for (curClockCycle = 0; curClockCycle < maxClockCycle; curClockCycle++) { 
             for (int i = 0; i < numCores; i++) { allCores[i]->run(curClockCycle);}
         }
         exit ();

     }



     


};