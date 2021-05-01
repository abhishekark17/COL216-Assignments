#ifndef DRAM_HPP
#define DRAM_HPP
#include "main.hpp"
#include "Request.hpp"
#include "output.hpp"

class Request;
class DRAM {
private:
    vector<int> * memory; 
    static const int numDramRows = 1024, numDramCols = 1024;
    int currentRow, currentCol;
    static pair<int, int> getActualRowColFromAddress(int address);

    int loadedValueForLW, valueToBeStoredForSW;
    OutputHandler * handleOutput;
public:
    static int getRowOfRowBuffer(int address);
    static int getColOfRowBuffer(int address);
    


    DRAM (OutputHandler * ho);
    int setMemory (int address, int value);
    int getMemory (int address);

    int getLoadedValueForLW ();
    int getValueToBeStoredForSW ();

    int lw(Request* r);
    int sw(Request * r);

    void setRowBuffer (int row) {currentRow = row;}
    int getRowBuffer () {return currentRow;}
    void setColBuffer (int col) {currentCol = col;}
    int getColBuffer () {return currentCol;}
    int getNumDRAMCols() {return numDramCols;}
    int getNumDRAMRows() {return numDramRows;}
    ~DRAM();
    

};

#endif