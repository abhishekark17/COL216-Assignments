#include "main.hpp"

class Request {
public:
    int cost;
    int instructionAddress;
    int core_id;

    Request (int c, int ia, int ci) {
        cost = c;
        instructionAddress = ia;
        core_id = ci;
    }
};