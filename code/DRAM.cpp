#include "DRAM.hpp"

pair<int, int> DRAM::getActualRowColFromAddress(int address)
{ // address supplied should be a multiple of 4
    pair<int, int> mypair;
    if (address < 0)
    {
        mypair.first = -1;
        mypair.second = -1;
        return mypair;
    }
    else if (address % 4 != 0)
    {
        mypair.first = -2;
        mypair.second = -2;
        return mypair;
    }
    else if (address > (numDramRows * numDramCols - 4))
    { //(1024*1024 - 4) last accessible address
        mypair.first = -3;
        mypair.second = -3;
        return mypair;
    }
    mypair.first = address / numDramRows;                     // row of dram
    mypair.second = (address - (mypair.first) * numDramRows); // actual col of dram
    return mypair;
}

int DRAM::getRowOfRowBuffer(int address)
{
    pair<int, int> mypair = DRAM::getActualRowColFromAddress(address);
    return mypair.first;
}
int DRAM::getColOfRowBuffer(int address)
{
    pair<int, int> mypair = DRAM::getActualRowColFromAddress(address);
    return mypair.second;
}



int DRAM::sw(Request * r)
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


DRAM::DRAM (OutputHandler * ho) {
        memory = new vector<int> (1<<18,0);
        currentRow = -1, currentCol = -1;
        handleOutput = ho;
        loadedValueForLW = -1;
        valueToBeStoredForSW = -1;
    }
    int DRAM::setMemory (int address, int value) {
        if (address < 0 || address > 1048576) return -1;
        else if (address % 4 != 0) return -2;
        else address /= 4;
        memory->at(address) = value;
        return 0;
    }
    int DRAM::getMemory (int address) {
        if (address < 0 || address > 1048576) return -1;
        else if (address % 4 != 0) return -2;
        else address /= 4;
        return memory->at(address);
    }

    int DRAM::getLoadedValueForLW () {return loadedValueForLW;}
    int DRAM::getValueToBeStoredForSW () {return valueToBeStoredForSW;}

    int DRAM::lw(Request* r) 
    {
        int address = r->loadingMemoryAddress;
        cout << "loading memory address   " << address << endl;
        if (address < 0 || address > 1048576)
        {   
            return -1;
        }
        else if (address % 4 != 0)
        {
            return -2;
        }

        else address /= 4;
        loadedValueForLW = memory->at(address);
        cout << "loadedValue for lw " << loadedValueForLW << endl;
        return 0;
    }