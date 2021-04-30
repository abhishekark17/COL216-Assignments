

#ifndef Instruction_HPP
#define Instruction_HPP

#include "main.hpp"
class instruction {
  public:
    vector<int> cirs;
    int opID;
    string label="";

    vector<int> cirsValue; 
    
    instruction (vector<int> CIRS , int id) {
        cirs = CIRS;
        cirsValue = *(new vector<int>);
        opID = id;
    }
    instruction (vector<int> CIRS , int id,string str) {
        cirsValue = *(new vector<int>);
        cirs = CIRS;
        opID = id;
        label=str;
    }
    instruction () {
        cirsValue = *(new vector<int>);
        cirs = {-1,-1,-1,-1};
        opID = -1;
    }
};

#endif