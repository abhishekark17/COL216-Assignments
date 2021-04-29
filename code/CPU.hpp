#include "main.hpp"
#include "Core.hpp"
#include "DRAM.hpp"
#include "MRM.hpp"

class CPU {
private:
    int numCores, curClockCycle;
    int maxClockCycle;
    vector<CORE*> * allCores;
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
            allCores->push_back(new CORE (folderPath + "/" +to_string(i+1)+".txt", i+1));
        }

        dram = new DRAM ();
        memoryRequestManager = new MRM (dram,numCores);
        

        curClockCycle = 0;
     }

     void exit ();

     void run () {
         for (curClockCycle = 0; curClockCycle < maxClockCycle; curClockCycle++) { 
             Request* request = memoryRequestManager->execute(allCores,curClockCycle);
             for (int i = 0; i < numCores; i++) { 
                if (request->core_id == i + 1 && request->inst->opID != 7 && request->inst->opID != 8 && request->cost == 1) 
                    allCores->at(i)->run(curClockCycle,true,request);
            }
             
         }

         exit ();

     }



     


};