

#ifndef Request_HPP
#define Request_HPP
#include "Instruction.hpp"
#include "main.hpp"
#include "Core.hpp"
#include "output.hpp"

class CORE;
class Request {
public:
    double cost;
    CORE* coreOfRequest;
    instruction* inst;
    int core_id;
    int changingRegister;
    int loadingMemoryAddress, savingMemoryAddress;
    int storeThisForSW;

    Request (double c, int ci,instruction* i, CORE* cc);


};

#endif