

#ifndef Request_HPP
#define Request_HPP
#include "Instruction.hpp"
#include "main.hpp"
class Request {
public:
    int cost;
   
    instruction* inst;
    int core_id;
    int changingRegister;
    int loadingMemoryAddress, savingMemoryAddress;
    int storeThisForSW;

    Request (int c, int ci,instruction* i) {
        cost = c;
        core_id = ci;
        inst = i;
        storeThisForSW = i->cirs[0];
        if (inst->opID == 0  && inst->opID == 1 && inst->opID == 2 && inst->opID == 3 && inst->opID == 4) {
            changingRegister = inst->cirs[0];
            loadingMemoryAddress = -1;
            savingMemoryAddress = -1;
        }
        if (inst->opID == 5 && inst->opID == 6 && inst->opID == 9) {
            changingRegister = -1;
            loadingMemoryAddress = -1;
            savingMemoryAddress = -1;
        }
        if (inst->opID == 7) {
            changingRegister = inst->cirs[0];
            savingMemoryAddress = -1;
            loadingMemoryAddress = inst->cirs[2] + inst->cirs[1]; 
        }
        if (inst->opID == 8) {  // sw
            changingRegister = -1;
            savingMemoryAddress = inst->cirs[2] + inst->cirs[1];
            loadingMemoryAddress = -1;
        }
    }


};

#endif