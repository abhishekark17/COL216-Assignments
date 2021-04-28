#include "main.hpp"

class instruction {
  public:
    vector<int> cirs;
    int opID;
    string label="";

    vector<int> cirsValue; 
    
    instruction (vector<int> CIRS , int id) {
        cirs = CIRS;
        opID = id;
    }
    instruction (vector<int> CIRS , int id,string str) {
        cirs = CIRS;
        opID = id;
        label=str;
    }
    instruction () {
        cirs = {-1,-1,-1,-1};
        opID = -1;
    }
};