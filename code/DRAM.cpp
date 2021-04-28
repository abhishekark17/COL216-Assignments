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