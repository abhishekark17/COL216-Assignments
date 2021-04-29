#include "main.hpp"
#include "Instruction.hpp"
class Request {
public:
    int cost;
    int instructionAddress;
    instruction* inst;
    int core_id;

    Request (int c, int ia, int ci,instruction* i) {
        cost = c;
        instructionAddress = ia;
        core_id = ci;
        inst = i;
    }

};