#include "common.hpp"


bool ifLW(instruction &curInst, int address)
{
    return cannotChangeRegistersG.find(curInst.cirs[0]) == cannotChangeRegistersG.end() &&
           cannotUseRegistersG.find(curInst.cirs[0]) == cannotUseRegistersG.end() &&
           cannotChangeRegistersG.find(curInst.cirs[2]) == cannotChangeRegistersG.end() &&
           cannotChangeMemoryG.find(address) == cannotChangeMemoryG.end();
}
bool ifSW(instruction &curInst, int address)
{
    return cannotChangeRegistersG.find(curInst.cirs[0]) == cannotChangeRegistersG.end() &&
           cannotChangeRegistersG.find(curInst.cirs[2]) == cannotChangeRegistersG.end() &&
           cannotChangeMemoryG.find(address) == cannotChangeMemoryG.end() &&
           cannotUseMemoryG.find(address) == cannotUseMemoryG.end();
}
void eraseIfP_URG(int reg)
{
    auto itr = cannotUseRegistersG.find(reg);
    if (itr != cannotUseRegistersG.end())
        cannotUseRegistersG.erase(itr);
    return;
}
void eraseIfP_CRG(int reg)
{
    auto itr = cannotChangeRegistersG.find(reg);
    if (itr != cannotChangeRegistersG.end())
        cannotChangeRegistersG.erase(itr);
    return;
}
void eraseIfP_UMG(int mem)
{
    auto itr = cannotUseMemoryG.find(mem);
    if (itr != cannotUseMemoryG.end())
        cannotUseMemoryG.erase(itr);
    return;
}
void eraseIfP_CMG(int mem)
{
    auto itr = cannotChangeMemoryG.find(mem);
    if (itr != cannotChangeMemoryG.end())
        cannotChangeMemoryG.erase(itr);
    return;
}

void eraseIfP_LW(instruction curInst)
{
    eraseIfP_URG(curInst.cirs[0]);
    eraseIfP_CRG(curInst.cirs[2]);
    eraseIfP_CRG(curInst.cirs[0]);
    eraseIfP_CMG(getAddress(curInst));
    return;
}


void eraseIfP_SW(instruction curInst)
{
    eraseIfP_CRG(curInst.cirs[0]);
    eraseIfP_CRG(curInst.cirs[2]);
    eraseIfP_CMG(getAddress(curInst));
    eraseIfP_UMG(getAddress(curInst));
    return;
}

int getAddress(instruction &curInst)
{
    int offset = curInst.cirs[1];
    int typeOfarg = curInst.cirs[3];
    int address;
    if (typeOfarg == 0)
    {
        address = offset + registers[curInst.cirs[2]];
        blockRegisterID = curInst.cirs[2];
    }
    else
    {
        address = offset + curInst.cirs[2];
        blockMemoryAdd = address;
    }
    return address;
}

void intializerowToInsSet()
{
    for (auto &x : InstAddToBlocks)
    {
        int instructionAdd = x.first;
        if (InstAddToFreq[instructionAdd] > 0)
        {
            instruction curInst = iset[instructionAdd - 1];
            int address = getAddress(curInst);
            int wantedRow = getRowOfRowBuffer(address);
            rowToInsSet[wantedRow].insert(instructionAdd);
        }
    }
}

void removeInstFromDeque(deque<int> &mydeq, int insAddr)
{
    deque<int>::iterator itr = mydeq.begin();
    while (itr != mydeq.end())
    {
        if (*itr == insAddr)
        {
            mydeq.erase(itr);
            break;
        }
        itr++;
    }
    return;
}

void eraseAllBlocks(int insAddr)
{
    vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[insAddr];
    // vector[0] = cannotUseRegisters;
    // vector[1] = cannotChangeRegisters;
    // vector[2] = cannotUseMemory;
    // vector[3] = cannotChangeMemory;
    for (int reg : blockedByInsAddr[0])
    {
        auto itr = cannotUseRegistersG.find(reg);
        if (itr != cannotUseRegistersG.end())
            cannotUseRegistersG.erase(itr);
    }
    for (int reg : blockedByInsAddr[1])
    {
        auto itr = cannotChangeRegistersG.find(reg);
        if (itr != cannotChangeRegistersG.end())
            cannotChangeRegistersG.erase(itr);
    }
    for (int mem : blockedByInsAddr[2])
    {
        auto itr = cannotUseMemoryG.find(mem);
        if (itr != cannotUseMemoryG.end())
            cannotUseMemoryG.erase(itr);
    }
    for (int mem : blockedByInsAddr[3])
    {

        auto itr = cannotChangeMemoryG.find(mem);
        if (itr != cannotChangeMemoryG.end())
            cannotChangeMemoryG.erase(itr);
    }
    InstAddToFreq[insAddr] = max(InstAddToFreq[insAddr] - 1, 0);
}
