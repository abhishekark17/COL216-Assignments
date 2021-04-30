#ifndef DRAM_HPP
#define DRAM_HPP
#include "main.hpp"
#include "Request.hpp"

class DRAM {
private:
    vector<int> * memory; 
    static const int numDramRows = 1024, numDramCols = 1024;
    int currentRow, currentCol;
    static pair<int, int> getActualRowColFromAddress(int address);

    int loadedValueForLW, valueToBeStoredForSW;
public:
    static int getRowOfRowBuffer(int address);
    static int getColOfRowBuffer(int address);
    


    DRAM () {
        memory = new vector<int> (1<<18,0);
        currentRow = -1, currentCol = -1;
    }
    void setMemory (int address, int value) {
        memory->at(address) = value;
    }
    int getMemory (int address) {
        return memory->at(address);
    }

    int getLoadedValueForLW () {return loadedValueForLW;}
    int getValueToBeStoredForSW () {return valueToBeStoredForSW;}

    int lw(Request* r) 
    {
        int address = r->loadingMemoryAddress;
        if (address < 0 || address > 1048576)
        {
            //core->setRuntimeError("");
            return -1;
        }
        else if (address % 4 != 0)
        {
            //core->setRuntimeError("");
            return -2;
        }
        else address /= 4;
        loadedValueForLW = memory->at(address);
        return 0;
    }

    int sw(Request * r)
    {
        int address = r->savingMemoryAddress;
        if (address < 0 || address > 1048576)
        {
            
            return -1;
        }
        else if (address % 4 != 0)
        {
            return -2;
        }
        else address /= 4;
        valueToBeStoredForSW = r->storeThisForSW;
        return 0;
    }

    void setRowBuffer (int row) {currentRow = row;}
    int getRowBuffer () {return currentRow;}
    void setColBuffer (int col) {currentCol = col;}
    int getColBuffer () {return currentCol;}
    int getNumDRAMCols() {return numDramCols;}
    int getNumDRAMRows() {return numDramRows;}
    ~DRAM();
    

};

#endif