#include "main.hpp"
#include "Core.hpp"
#include "DRAM.hpp"

class MRM {
private:
    DRAM * dram;
    bool DRAMrequestIssued;

public:
    MRM (DRAM * d, int numCores) {
        dram = d;
        DRAMrequestIssued = false;
    }

    void selectCore (vector<CORE*> * allCores) {
        int minCost = INT_MAX;
        CORE * coreWithMinCost = nullptr;
        for (auto core = allCores->begin(); core != allCores->end(); core++) {
            if (core->getMinCost() < minCost) {
                minCost = core->getMinCost();
                coreWithMinCost = core;
            }
        }
    }

    



};