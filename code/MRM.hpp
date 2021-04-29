#include "main.hpp"
#include "Core.hpp"
#include "DRAM.hpp"
#include "Request.hpp"
class MRM {
private:
    DRAM * dram;
    bool DRAMrequestIssued;
    int uptoClkCycle; 

public:
    MRM (DRAM * d, int numCores) {
        dram = d;
        DRAMrequestIssued = false;
    }

    CORE * selectCore (vector<CORE*> * allCores) {
        // * returns nullptr if there are no requests 
        int minCost = INT_MAX;
        CORE * coreWithMinCost = nullptr;
        for (auto core = allCores->begin(); core != allCores->end(); core++) {
            if ((*core)->getMinCost() < minCost) {
                minCost = (*core)->getMinCost();
                coreWithMinCost = (*core);
            }
        }
        return coreWithMinCost;
    }

    Request* getMinCostRequest (vector<CORE*> * allCores) {    
        Request* request = (selectCore (allCores))->getRequestWithMinCost();
        return request;
    }

    Request* execute (vector<CORE*> * allCores, int currentClockCycle) {
        Request* request = getMinCostRequest (allCores);
        if (request == nullptr) return nullptr;

        if (request->inst->opID != 7 && request->inst->opID != 8 && request->cost <= 1) return request;
        if (request->inst->opID != 7 && request->inst->opID != 8 && request->cost > 1) ERROR;
        
        if (DRAMrequestIssued) {
            // TODO: decide which request to execute next;
            if (currentClockCycle > uptoClkCycle) DRAMrequestIssued = false;
        }
        else {
            DRAMrequestIssued = true;
            

            
        }
    }



    



};