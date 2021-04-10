#include "a4p1.hpp"

// * assume that inside bracket always register.
/* *
* lw: r1, offset, r2
* r1 cannot change, cannot use
* r2 cannot change, but can use r2
* mem address (offset + r2) cannot change, but can use mem address

* sw: r1, offset, r2
* r1 cannot change, but can use r1
* r2 cannot change, but can use r2
* mem address (offset + r2) cannot change, cannot use 
*/

void exeLW(ofstream &out, instruction currInst, int instAddr,bool erase);
void exeAllBlkingInstLW(ofstream &out, instruction currInst, int upto,int ia);
void exeAllBlkingInstSW(ofstream &out, instruction currInst, int upto,int ia);
int getAddress(instruction &curInst);

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

void exeLW(ofstream &out, instruction currInst, int instAddr, bool erase)
{

    // cout << "instAddr inside exeLW :" << instAddr << endl;
    int address = getAddress(currInst);
    int wantedRow = getRowOfRowBuffer(address);
    if (wantedRow < 0)
        error1 = "BAD ADDRESS (lw)";
    if (erase) eraseIfP_LW(currInst);
    // cout << "before exeAllBlkingInstLW" << endl;
    exeAllBlkingInstLW(out, currInst, 2,instAddr);
    // cout << "after exeAllBlkingInstLW" << endl;
    int prevRow = currentRowInRowBuffer;
    DRAMrequestIssued = true;
    exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (lw)");

    if (currentRowInRowBuffer == wantedRow)
        uptoClkCyc = numClockCycles + COL_ACCESS_DELAY;
    else
    {
        if (currentRowInRowBuffer == -1)
            uptoClkCyc = numClockCycles + ROW_ACCESS_DELAY + COL_ACCESS_DELAY;
        else
            uptoClkCyc = numClockCycles + (2 * ROW_ACCESS_DELAY) + COL_ACCESS_DELAY;
        currentRowInRowBuffer = wantedRow;
        numRowBufferUpdates++;
    }

    lw(currInst.cirs);
    InstAddToFreq[instAddr] = max(InstAddToFreq[instAddr] - 1, 0);
    if (prevRow == -1)
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
    }
    else if (prevRow == wantedRow)
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
    else
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "WriteBack Row " + to_string(prevRow) + " to DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY)) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY) + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
    }
    ////exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1)  + "-" + to_string(uptoClkCyc) + ": " +  rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]) );
    numOfInst[7]++;
}

void exeSW(ofstream &out, instruction currInst, int instAddr, bool erase)
{
    // cout << "entered exeSw " << endl;
    int address = getAddress(currInst);
    int wantedRow = getRowOfRowBuffer(address);
    if (wantedRow < 0)
        error1 = "BAD ADDRESS (sw)";
    if (erase) eraseIfP_SW(currInst);
    exeAllBlkingInstSW(out, currInst, 2,instAddr);
    DRAMrequestIssued = true;
    int prevRow = currentRowInRowBuffer;
    exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (sw)");

    if (currentRowInRowBuffer == wantedRow)
        uptoClkCyc = numClockCycles + COL_ACCESS_DELAY;
    else
    {
        if (currentRowInRowBuffer == -1)
            uptoClkCyc = numClockCycles + ROW_ACCESS_DELAY + COL_ACCESS_DELAY;
        else
            uptoClkCyc = numClockCycles + (2 * ROW_ACCESS_DELAY) + COL_ACCESS_DELAY;
        currentRowInRowBuffer = wantedRow;
        numRowBufferUpdates++;
    }

    sw(currInst.cirs);
    InstAddToFreq[instAddr] = max(InstAddToFreq[instAddr] - 1, 0);
    numRowBufferUpdates++;

    if (prevRow == -1)
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": memory address " + to_string(address) + "-" + to_string(address + 3) + " = " + to_string(registers[currInst.cirs[0]]));
    }
    else if (prevRow == wantedRow)
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": memory address " + to_string(address) + "-" + to_string(address + 3) + " = " + to_string(registers[currInst.cirs[0]]));

    else
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "WriteBack Row " + to_string(prevRow) + " to DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY)) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY) + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": memory address " + to_string(address) + "-" + to_string(address + 3) + " = " + to_string(registers[currInst.cirs[0]]));
    }
    numOfInst[8]++;
    // cout << "exited exeSw " << endl;
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

void removeAndExeOPID7(ofstream &out, instruction &curInst, int insAddr)
{
    if (iset[insAddr - 1].opID == 7 && InstAddToFreq[insAddr] > 0)
    {
        //eraseIfP_LW(curInst);
        exeLW(out, iset[insAddr - 1], insAddr,true);
        
        //eraseAllBlocks(instructionAdd);
    }
}

void removeAndExeOPID8(ofstream &out, instruction &curInst, int insAddr)
{
    // cout << "entered remove and execute opid 8" << endl;
    if (iset[insAddr - 1].opID == 8 && InstAddToFreq[insAddr] > 0)
    {
        //eraseIfP_SW(curInst);
        // cout << "called exeSw and currinst address was " << insAddr << endl;
        exeSW(out, iset[insAddr - 1], insAddr,true);
        
        //eraseAllBlocks(instructionAdd);
    }
    // cout << "exited remove and execute opid 8" << endl;
}

void exeAllBlkingInstAdd(ofstream &out, instruction currInst, int upto)
{
    for (int regPos = 0; regPos <= upto; regPos++)
    {
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end())
        {
            //eraseIfP_URG(currInst.cirs[regPos]);
            deque<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            deque<int> cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[regPos]];
            while (!ublkRegQueue.empty())
            {
                int insAddr = ublkRegQueue.front();
                ublkRegQueue.pop_front();
                removeInstFromDeque(cblkRegQueue, insAddr);
                cBlockRegToInstAddQueue[currInst.cirs[regPos]] = cblkRegQueue;
                uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;

                removeAndExeOPID7(out, currInst, insAddr);
                removeAndExeOPID8(out, currInst, insAddr);
                ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
                cblkRegQueue=cBlockRegToInstAddQueue[currInst.cirs[regPos]];

                //eraseAllBlocks(insAddr);
                if (DRAMrequestIssued)
                {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                //ublkRegQueue.pop();
            }
        }
    }
    if (cannotChangeRegistersG.find(currInst.cirs[0]) != cannotChangeRegistersG.end())
    {
        //eraseIfP_CRG(currInst.cirs[0]);
        deque<int> cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];
        deque<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[0]];
        while (!cblkRegQueue.empty())
        {
            int insAddr = cblkRegQueue.front();
            cblkRegQueue.pop_front();
            cBlockRegToInstAddQueue[currInst.cirs[0]] = cblkRegQueue;
            removeInstFromDeque(ublkRegQueue, insAddr);
            //ublkRegQueue.erase(remove(ublkRegQueue.begin(), ublkRegQueue.end(), insAddr), ublkRegQueue.end());
            uBlockRegToInstAddQueue[currInst.cirs[0]] = ublkRegQueue;

            removeAndExeOPID7(out, currInst, insAddr);
            removeAndExeOPID8(out, currInst, insAddr);
            cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];
            //eraseAllBlocks(insAddr);
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
        }
    }
}

void exeAllBlkingInstLW(ofstream &out, instruction currInst, int upto, int ia)
{
    //eraseIfP_LW(currInst);
    // cout << "entered exeAllBlkingInstLW for ia " << ia << endl;
    for (int regPos = 0; regPos <= upto; regPos += 2)
    {
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end())
        {
            //cout << "should not enter here" <<endl;
            // cout << "caught cannot use registers G inside eabilw :" << rrmap[currInst.cirs[regPos]] << endl;
            // eraseIfP_URG(currInst.cirs[regPos]);
            // if (regPos == 0)
            //     eraseIfP_CRG(currInst.cirs[regPos]);

            deque<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            deque<int> cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[regPos]];
            bool removedself = false;
            while (!ublkRegQueue.empty())
            {
                int insAddr = ublkRegQueue.front();
                if (removedself == false && ia == insAddr) {
                ublkRegQueue.pop_front();
                removeInstFromDeque(cblkRegQueue, insAddr);
                removedself = true;
                }  
                uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
                cBlockRegToInstAddQueue[currInst.cirs[regPos]] = cblkRegQueue;
                if (!ublkRegQueue.empty()) {
                    ublkRegQueue.pop_front();
                    removeInstFromDeque(cblkRegQueue, insAddr);
                    uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
                    cBlockRegToInstAddQueue[currInst.cirs[regPos]] = cblkRegQueue;

                    removeAndExeOPID7(out, currInst, insAddr);
                    removeAndExeOPID8(out, currInst, insAddr);
                    ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
                    cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[regPos]];
                    //eraseAllBlocks(insAddr);
                    if (DRAMrequestIssued)
                    {
                        numClockCycles = uptoClkCyc + 1;
                        DRAMrequestIssued = false;
                    }
                    //ublkRegQueue.pop();
                }
                
            }
        }
    }

    if (cannotChangeRegistersG.find(currInst.cirs[0]) != cannotChangeRegistersG.end())
    {
        // cout << "should print caught cannot change register : " << rrmap[currInst.cirs[0]] << endl;
        // eraseIfP_CRG(currInst.cirs[0]);
        // eraseIfP_URG(currInst.cirs[0]);

        deque<int> cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];
        deque<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[0]];
        bool removedself = false;
        while (!cblkRegQueue.empty())
        {
            
            int insAddr = cblkRegQueue.front();
            
            if (removedself == false && ia == insAddr) {
                cblkRegQueue.pop_front();
                removeInstFromDeque(ublkRegQueue, insAddr);
                removedself = true;
            }
            uBlockRegToInstAddQueue[currInst.cirs[0]] = ublkRegQueue;
            cBlockRegToInstAddQueue[currInst.cirs[0]] = cblkRegQueue;
            if (!cblkRegQueue.empty()) {
                insAddr = cblkRegQueue.front();
                // cout << insAddr << " front instruction address   op id :" << currInst.opID   << endl;
                cblkRegQueue.pop_front();
                removeInstFromDeque(ublkRegQueue, insAddr);
                uBlockRegToInstAddQueue[currInst.cirs[0]] = ublkRegQueue;
                cBlockRegToInstAddQueue[currInst.cirs[0]] = cblkRegQueue;
                //cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];

                removeAndExeOPID7(out, currInst, insAddr);
                removeAndExeOPID8(out, currInst, insAddr);
                cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];
                ublkRegQueue=uBlockRegToInstAddQueue[currInst.cirs[0]];
                //eraseAllBlocks(insAddr);
                if (DRAMrequestIssued)
                {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
            }
            
        }
    }

    if (cannotUseMemoryG.find(getAddress(currInst)) != cannotUseMemoryG.end())
    {
        //eraseIfP_UMG(getAddress(currInst));
        deque<int> ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
        bool removedself = false;
        while (!ublkMemQueue.empty())
        {
            int insAddr = ublkMemQueue.front();
            if (removedself == false && ia == insAddr) {
                ublkMemQueue.pop_front();
                removedself = true;
            }
            uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;
            if (!ublkMemQueue.empty()) {
                int insAddr = ublkMemQueue.front();
                ublkMemQueue.pop_front();
                uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;

                removeAndExeOPID7(out, currInst, insAddr);
                removeAndExeOPID8(out, currInst, insAddr);
                //eraseAllBlocks(insAddr);
                ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
                if (DRAMrequestIssued)
                {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
            }
            
        }
    }
    // cout << "exited exeAllBlkingInstLW for ia :" << ia << endl;
}

void exeAllBlkingInstSW(ofstream &out, instruction currInst, int upto,int ia)
{
    //eraseIfP_SW(currInst);
    for (int regPos = 0; regPos <= upto; regPos += 2)
    {
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end())
        {
            // cout << "caught cannot change register inside sw : " << rrmap[currInst.cirs[regPos]] << endl;
            // eraseIfP_URG(currInst.cirs[regPos]);
            deque<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            bool removedself = false;
            while (!ublkRegQueue.empty())
            {
                int insAddr = ublkRegQueue.front();

                if (removedself == false && ia == insAddr) {
                    removedself = true;
                    ublkRegQueue.pop_front();
                }
                uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
                if (!ublkRegQueue.empty()) {
                    int insAddr = ublkRegQueue.front();
                    ublkRegQueue.pop_front();
                    uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;

                    removeAndExeOPID7(out, currInst, insAddr);
                    removeAndExeOPID8(out, currInst, insAddr);
                    ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
                    //eraseAllBlocks(insAddr);
                    if (DRAMrequestIssued)
                    {
                        numClockCycles = uptoClkCyc + 1;
                        DRAMrequestIssued = false;
                    }
                }
                
            }
        }
    }

    if (cannotUseMemoryG.find(getAddress(currInst)) != cannotUseMemoryG.end())
    {
        //eraseIfP_UMG(getAddress(currInst));

        deque<int> ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
        deque<int> cblkMemQueue = cBlockMemoryToInstAddQueue[getAddress(currInst)];
        bool removedself = false;
        while (!ublkMemQueue.empty())
        {
            int insAddr = ublkMemQueue.front();
            if (removedself == false && ia == insAddr) {
                    removedself = true;
                    ublkMemQueue.pop_front();
                    removeInstFromDeque(cblkMemQueue, insAddr);
            }
            cBlockMemoryToInstAddQueue[getAddress(currInst)] = cblkMemQueue;
            uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;
            if (!ublkMemQueue.empty()) {
                int insAddr = ublkMemQueue.front();
                ublkMemQueue.pop_front();

                uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;
                removeInstFromDeque(cblkMemQueue, insAddr);
                cBlockMemoryToInstAddQueue[getAddress(currInst)] = cblkMemQueue;

                removeAndExeOPID7(out, currInst, insAddr);
                removeAndExeOPID8(out, currInst, insAddr);
                ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
                cblkMemQueue = cBlockMemoryToInstAddQueue[getAddress(currInst)];
                //eraseAllBlocks(insAddr);
                if (DRAMrequestIssued)
                {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
            }
            
        }
    }

    if (cannotChangeMemoryG.find(getAddress(currInst)) != cannotChangeMemoryG.end())
    {
        //eraseIfP_CMG(getAddress(currInst));
        deque<int> ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
        deque<int> cblkMemQueue = cBlockMemoryToInstAddQueue[getAddress(currInst)];
        bool removedself = false;
        while (!cblkMemQueue.empty())
        {
            int insAddr = cblkMemQueue.front();
            if (removedself == false && insAddr == ia) {
                removedself = true;
                cblkMemQueue.pop_front();
                removeInstFromDeque(ublkMemQueue, insAddr);
            }
            cBlockMemoryToInstAddQueue[getAddress(currInst)] = cblkMemQueue;
            uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;
            if (!cblkMemQueue.empty()) {
                int insAddr = cblkMemQueue.front();
                cblkMemQueue.pop_front();
                removeInstFromDeque(ublkMemQueue, insAddr);
                cBlockMemoryToInstAddQueue[getAddress(currInst)] = cblkMemQueue;
                uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;

                removeAndExeOPID7(out, currInst, insAddr);
                removeAndExeOPID8(out, currInst, insAddr);

                //eraseAllBlocks(insAddr);
                cblkMemQueue = cBlockMemoryToInstAddQueue[getAddress(currInst)];
                ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
                if (DRAMrequestIssued)
                {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
            }
            
        }
    }
}

void exeAllBlkingInstSub(ofstream &out, instruction currInst, int upto) { exeAllBlkingInstAdd(out, currInst, upto); }
void exeAllBlkingInstMul(ofstream &out, instruction currInst, int upto) { exeAllBlkingInstAdd(out, currInst, upto); }
void exeAllBlkingInstSlt(ofstream &out, instruction currInst, int upto) { exeAllBlkingInstAdd(out, currInst, upto); }
void exeAllBlkingInstAddi(ofstream &out, instruction currInst, int upto) { exeAllBlkingInstAdd(out, currInst, upto); }
void exeAllBlkingInstBne(ofstream &out, instruction currInst, int upto)
{
    for (int regPos = 0; regPos <= upto; regPos++)
    {
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end())
        {
            // eraseIfP_URG(currInst.cirs[regPos]);
            deque<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            
            while (!ublkRegQueue.empty())
            {
                int insAddr = ublkRegQueue.front();
                ublkRegQueue.pop_front();
                uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;

                removeAndExeOPID7(out, currInst, insAddr);
                removeAndExeOPID8(out, currInst, insAddr);
                //eraseAllBlocks(insAddr);
                ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
                if (DRAMrequestIssued)
                {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                //ublkRegQueue.pop();
            
        
            }
        }
    }
}
void exeAllBlkingInstBeq(ofstream &out, instruction currInst, int upto) { exeAllBlkingInstBne(out, currInst, upto); }

void execute1a4(ofstream &out)
{
    while (pc <= maxInstructions)
    {
        if (error1 != "")
            return;

        numClockCycles++;
        if (numClockCycles > uptoClkCyc)
            DRAMrequestIssued = false;
    
        instruction currInst = iset[pc - 1];
        pc++;

        switch (currInst.opID)
        {
        case 0:
        { // *add
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            exeAllBlkingInstAdd(out, currInst, 2);
            add(currInst.cirs);

            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: add (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[0]++;

            break;
        }
        case 1:
        { // *sub
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            exeAllBlkingInstSub(out, currInst, 2);

            sub(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: sub (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[1]++;

            break;
        }
        case 2:
        { // *mul
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            exeAllBlkingInstMul(out, currInst, 2);
            mul(currInst.cirs);

            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: Mul (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[2]++;
            break;
        }
        case 3:
        { // *slt
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            exeAllBlkingInstSlt(out, currInst, 2);

            slt(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: slt (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[3]++;
            break;
        }

        case 4:
        { // *addi
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }

            exeAllBlkingInstAddi(out, currInst, 1);

            addi(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: addi (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + to_string(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[4]++;

            break;
        }

        // *NOTE THAT WHENEVER YOU ARE BRANCHING, YOU HAVE TO WAIT IF THE REGISTERS ARE NOT AVAILABLE BECAUSE YOU CANNOT DECIDE WHICH INSTRUCTION TO EXECUTE FIRST
        // *HENCE WAIT UNTILL ALL CLOCK CYCLES TO LOAD THE NECESSARY REGISTER AND THEN EXECUTE THE CURRENT COMMAND.
        case 5:
        { // *bne
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            exeAllBlkingInstBne(out, currInst, 1);

            bne(currInst.cirs, currInst.label);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction bne (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + currInst.label + "," + to_string(switchOnBranch) + ")");
            numOfInst[5]++;
            break;
        }
        case 6:
        { // *beq
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            exeAllBlkingInstBeq(out, currInst, 1);

            beq(currInst.cirs, currInst.label);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction beq (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + currInst.label + "," + to_string(switchOnBranch) + ")");
            numOfInst[6]++;
            break;
        }
        case 7:
        { //lw
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            
            int address = getAddress(currInst);
            int prevRow = currentRowInRowBuffer;
            int wantedRow = getRowOfRowBuffer(address);
            // * Now we can see if we want to execute it now or not
            if (wantedRow < 0)
                error1 = "BAD ADDRESS (lw)";
            else if (currentRowInRowBuffer == -1 || (currentRowInRowBuffer == wantedRow &&
                                                     ifLW(currInst, address)))
            { // * Here we should execute immediately.
                exeLW(out, currInst, pc - 1,false);
            }
            else // * even if row buffer is same, if some registers are blocked, then we will need to execute
                // * the blocked instructions first, which may change the row buffer, hence do not execute if some are blocked.
            { //* We do not need to execute it currently. postpone.
                numClockCycles--;
                cannotChangeMemory.clear();
                cannotChangeRegisters.clear();
                cannotUseMemory.clear();
                cannotChangeRegisters.clear();

                cannotChangeMemory.insert(address);
                cannotChangeRegisters.insert(currInst.cirs[2]);
                cannotChangeRegisters.insert(currInst.cirs[0]);
                cannotUseRegisters.insert(currInst.cirs[0]);

                cannotChangeMemoryG.insert(address);
                cannotChangeRegistersG.insert(currInst.cirs[2]);
                cannotChangeRegistersG.insert(currInst.cirs[0]);
                cannotUseRegistersG.insert(currInst.cirs[0]);

                vector<unordered_set<int>> temp(4);

                temp[0] = cannotUseRegisters;
                temp[1] = cannotChangeRegisters;
                temp[2] = cannotUseMemory;
                temp[3] = cannotChangeMemory;

                InstAddToBlocks[pc - 1] = temp;
                InstAddToFreq[pc - 1]++;
                uBlockRegToInstAddQueue[currInst.cirs[0]].push_back(pc - 1);
                cBlockRegToInstAddQueue[currInst.cirs[0]].push_back(pc - 1);
                cBlockRegToInstAddQueue[currInst.cirs[2]].push_back(pc - 1);
                cBlockMemoryToInstAddQueue[address].push_back(pc - 1);
            }
            break;
        }

        case 8:
        { //sw
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }

            int address = getAddress(currInst);
            int wantedRow = getRowOfRowBuffer(address);
            int prevRow = currentRowInRowBuffer;
            if (wantedRow < 0)
                error1 = "BAD ADDRESS (sw)";
            else if (currentRowInRowBuffer == -1 || (currentRowInRowBuffer == wantedRow &&
                                                     ifSW(currInst, address)))
            { // * can execute similar to lw case
                exeSW(out, currInst, pc - 1, false);
            }

            else
            { // * postpone similar to lw case

                numClockCycles--;
                cannotChangeMemory.clear();
                cannotChangeRegisters.clear();
                cannotUseMemory.clear();
                cannotChangeRegisters.clear();

                cannotChangeMemory.insert(address);
                cannotUseMemory.insert(address);
                cannotChangeRegisters.insert(currInst.cirs[2]);
                cannotChangeRegisters.insert(currInst.cirs[0]);

                cannotChangeMemoryG.insert(address);
                cannotUseMemoryG.insert(address);
                cannotChangeRegistersG.insert(currInst.cirs[2]);
                cannotChangeRegistersG.insert(currInst.cirs[0]);

                vector<unordered_set<int>> temp(4);

                temp[0] = cannotUseRegisters;
                temp[1] = cannotChangeRegisters;
                temp[2] = cannotUseMemory;
                temp[3] = cannotChangeMemory;
                
                InstAddToBlocks[pc - 1] = temp;
                InstAddToFreq[pc - 1]++;
                cBlockRegToInstAddQueue[currInst.cirs[0]].push_back(pc - 1);
                cBlockRegToInstAddQueue[currInst.cirs[2]].push_back(pc - 1);
                cBlockMemoryToInstAddQueue[address].push_back(pc - 1);
                uBlockMemoryToInstAddQueue[address].push_back(pc - 1);
            }
            break;
        }

        case 9:
        { //j       // note that here j instruction needs to wait before the DRAM calls are finished.
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            j(currInst.cirs, currInst.label);
            numOfInst[9]++;
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction j :" + "Jump to label ID: " + currInst.label);
            break;
        }
        };
        if (registers[0] != 0)
            error1 = "Zero register cannot be modified";
    }

    numClockCycles++;



    int counter = 0;


    for (auto &x : InstAddToBlocks)
    {
        int instructionAdd = x.first;
        if (InstAddToFreq[instructionAdd] > 0)
        {
            instruction curInst = iset[instructionAdd - 1];
            int address = getAddress(curInst);
            int wantedRow = getRowOfRowBuffer(address);
            if (currentRowInRowBuffer == wantedRow)
            {
                bool done = true;
                while (done && InstAddToFreq[instructionAdd] > 0)
                {
                    if (iset[instructionAdd - 1].opID == 7)
                    {
                        eraseIfP_LW(curInst);
                        if (ifLW(curInst, address))
                        {
                            exeLW(out, iset[instructionAdd - 1], instructionAdd,false);
                            //eraseAllBlocks(instructionAdd);
                        }
                    }
                    else if (iset[instructionAdd - 1].opID == 8)
                    {
                        eraseIfP_SW(curInst);
                        if (ifSW(curInst, address))
                        {
                            exeSW(out, iset[instructionAdd - 1], instructionAdd,false);
                            
                            //eraseAllBlocks(instructionAdd);
                        }
                    }
                    else
                        done = false;

                    if (InstAddToFreq[instructionAdd] == 0)
                    {
                        vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[instructionAdd];
                        for (int reg : blockedByInsAddr[0])
                            cannotUseRegistersG.erase(reg);
                        for (int reg : blockedByInsAddr[1])
                            cannotChangeRegistersG.erase(reg);
                        for (int mem : blockedByInsAddr[2])
                            cannotUseMemoryG.erase(mem);
                        for (int mem : blockedByInsAddr[3])
                            cannotChangeMemoryG.erase(mem);
                    }
                    //InstAddToFreq[instructionAdd]--;
                    if (DRAMrequestIssued)
                    {
                        numClockCycles = uptoClkCyc + 1;
                        DRAMrequestIssued = false;
                    }
                }
            }
        }
        //
    }

    // intializerowToInsSet();

    // for (auto &x : rowToInsSet)
    // {
    //     int rownum = x.first;
    //     for (auto &instructionAdd : rowToInsSet[rownum])
    //     {
    //         instruction curInst = iset[instructionAdd - 1];
    //         int address = getAddress(curInst);
    //         bool done = true;
            //cout << "counter at pos : a :" << counter << endl; counter++; 
    //         while (done && InstAddToFreq[instructionAdd] > 0)
    //         {
                //cout << "counter at pos : b :" << counter << endl; counter++; 
    //             if (iset[instructionAdd - 1].opID == 7)
    //             {
                   // cout << "counter at pos : c :" << counter << endl; counter++; 
    //                 bool flag=false;
    //                 multiset<int> tempCRG=cannotChangeRegistersG;
    //                 multiset<int> tempURG=cannotUseRegistersG;
    //                 multiset<int> tempCMG=cannotChangeMemoryG;
    //                 multiset<int> tempUMG=cannotUseMemoryG;
                   // for (auto x : cannotChangeRegistersG) cout << "printing here before :" << rrmap[x] << " insaddr :" << instructionAdd << endl;
    //                 eraseIfP_LW(curInst);
                    //for (auto x : cannotChangeRegistersG) cout << "printing here after:" << rrmap[x] << " insaddr :" << instructionAdd << endl;
    //                 if (ifLW(curInst, address))
    //                 {
                        //cout << "counter at pos : d :" << counter << endl; counter++; 
    //                     flag=true;
    //                     exeLW(out, iset[instructionAdd - 1], instructionAdd);
    //                     InstAddToFreq[instructionAdd]--;
    //                     //eraseAllBlocks(instructionAdd);
    //                 }
    //                 if(!flag){
    //                     cannotChangeRegistersG=tempCRG;
    //                     cannotUseRegistersG=tempURG;
    //                     cannotChangeMemoryG=tempCMG;
    //                     cannotUseMemoryG=tempUMG;
    //                     done = false;
    //                 }
    //             }

    //             else if (iset[instructionAdd - 1].opID == 8)
    //             {
                    //cout << "counter at pos : e :" << counter << endl; counter++; 
    //                 bool flag=false;
    //                 multiset<int> tempCRG=cannotChangeRegistersG;
    //                 multiset<int> tempURG=cannotUseRegistersG;
    //                 multiset<int> tempCMG=cannotChangeMemoryG;
    //                 multiset<int> tempUMG=cannotUseMemoryG;
    //                 eraseIfP_SW(curInst);
    //                 if (ifSW(curInst, address))
    //                 {
                        //cout << "counter at pos : f :" << counter << endl; counter++;
    //                     flag=true; 
    //                     exeSW(out, iset[instructionAdd - 1], instructionAdd);
    //                     InstAddToFreq[instructionAdd]--;
    //                     //eraseAllBlocks(instructionAdd);
    //                 }
    //                 if (!flag) {
    //                     cannotChangeRegistersG=tempCRG;
    //                     cannotUseRegistersG=tempURG;
    //                     cannotChangeMemoryG=tempCMG;
    //                     cannotUseMemoryG=tempUMG;
    //                     done = false;
    //                 }
    //             }
    //             else
    //                 done = false;

    //             if (InstAddToFreq[instructionAdd] == 0)
    //             {
    //                 vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[instructionAdd];
    //                 for (int reg : blockedByInsAddr[0])
    //                     cannotUseRegistersG.erase(reg);
    //                 for (int reg : blockedByInsAddr[1])
    //                     cannotChangeRegistersG.erase(reg);
    //                 for (int mem : blockedByInsAddr[2])
    //                     cannotUseMemoryG.erase(mem);
    //                 for (int mem : blockedByInsAddr[3])
    //                     cannotChangeMemoryG.erase(mem);
    //             }
    //             //InstAddToFreq[instructionAdd]--;
    //             if (DRAMrequestIssued)
    //             {
    //                 numClockCycles = uptoClkCyc + 1;
    //                 DRAMrequestIssued = false;
    //             }
    //         }
    //     }
    // }

    // * FINAL

    
    //for (auto &x : InstAddToBlocks)
    
  
    // rbegin() returns to the last value of map
    
    for (auto x = InstAddToBlocks.rbegin(); x != InstAddToBlocks.rend(); x++) 
    {
        int instructionAdd = x->first;
        while (InstAddToFreq[instructionAdd] > 0)
        {
            // cout << InstAddToFreq[instructionAdd] << " this is InstAddToFreq[instructionAdd]before"<< "  inst address  "<<instructionAdd << endl;
            // cout << "counter at pos : 1 :" << counter << endl; counter++;
            
            if (InstAddToFreq[instructionAdd] > 0 && iset[instructionAdd - 1].opID == 7){
                // cout << "counter at pos : 2 :" << counter << endl; counter++;
                //eraseIfP_LW(iset[instructionAdd - 1]);
                //exeLW(out, iset[instructionAdd - 1], instructionAdd,true);
                if (ifLW(iset[instructionAdd - 1],getAddress(iset[instructionAdd - 1]))) exeLW(out,iset[instructionAdd-1],instructionAdd,true);
                else {
                    eraseIfP_LW(iset[instructionAdd - 1]);
                    exeAllBlkingInstLW(out,iset[instructionAdd-1],2,instructionAdd);
                }
                // cout << InstAddToFreq[instructionAdd] << " this is InstAddToFreq[instructionAdd] after" << "  inst address  "<<instructionAdd << endl; 
                // cout << "counter at pos : 3 :" << counter << endl; counter++;
            }
                
            else if (InstAddToFreq[instructionAdd] > 0 && iset[instructionAdd - 1].opID == 8) {
                // cout << "counter at pos : 4 :" << counter << endl; counter++;
                //eraseIfP_SW(iset[instructionAdd - 1]);
                //exeSW(out, iset[instructionAdd - 1], instructionAdd,true);
                if (ifSW (iset[instructionAdd - 1],getAddress(iset[instructionAdd - 1]))) exeSW(out,iset[instructionAdd - 1],instructionAdd,true);
                else {
                    eraseIfP_SW(iset[instructionAdd - 1]);
                    exeAllBlkingInstSW(out,iset[instructionAdd-1],2,instructionAdd);
                }
                // cout << "counter at pos : 5 :" << counter << endl; counter++;
            }
                /*
            if (InstAddToFreq[instructionAdd] == 1)
            {
                vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[instructionAdd];
                for (int reg : blockedByInsAddr[0])
                    cannotUseRegistersG.erase(reg);
                for (int reg : blockedByInsAddr[1])
                    cannotChangeRegistersG.erase(reg);
                for (int mem : blockedByInsAddr[2])
                    cannotUseMemoryG.erase(mem);
                for (int mem : blockedByInsAddr[3])
                    cannotChangeMemoryG.erase(mem);
            }*/

            
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
        }
    }
    numClockCycles--;

    if (!(cannotUseMemoryG.empty() && cannotChangeMemoryG.empty() && cannotUseRegistersG.empty() && cannotChangeRegistersG.empty()))
        cout << "RUNTIME WARNING: All Registers and Memory Addresses should be free by now but some are busy\n";

    if (error1 != "")
        return;
    return;
}