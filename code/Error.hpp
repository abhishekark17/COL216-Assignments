#include "main.hpp"

class ErrorHandler 
{
private:
    int numCores;
    vector<string> CORE_ERRORS;
     
public:
    ErrorHandler(int nc) {
        numCores = nc;
    }
};