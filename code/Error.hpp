

#ifndef Error_HPP
#define Error_HPP
#include "main.hpp"

class ErrorHandler 
{
private:
    int numCores;
    vector<string> CORE_ERRORS;
     
public:
    ErrorHandler(int nc) {
        CORE_ERRORS  = *(new vector<string>);
        numCores = nc;
    }
};

#endif