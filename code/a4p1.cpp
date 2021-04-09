#include "a4p1.hpp"
//////////////////////////////////////////////////////////////////////////////////////////////////////////// THIS IS FOR PART 1 OF THE ASSIGNMENT ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void exeLW (ofstream &out,instruction currInst, int instAddr);
void exeAllBlkingInstLW (ofstream &out,instruction currInst, int upto);
void exeAllBlkingInstSW (ofstream &out,instruction currInst, int upto);
int getAddress (instruction& curInst);

bool ifLW (instruction& curInst,int address) {
    return  cannotChangeRegistersG.find (curInst.cirs[0]) == cannotChangeRegistersG.end() &&
            cannotUseRegistersG.find (curInst.cirs[0]) == cannotUseRegistersG.end() && 
            cannotChangeRegistersG.find (curInst.cirs[2]) == cannotChangeRegistersG.end() && 
            cannotChangeMemoryG.find(address) == cannotChangeMemoryG.end();
} 
bool ifSW (instruction & curInst, int address) {
    return  cannotChangeRegistersG.find (curInst.cirs[0]) == cannotChangeRegistersG.end() && 
            cannotChangeRegistersG.find (curInst.cirs[2]) == cannotChangeRegistersG.end() &&
            cannotChangeMemoryG.find(address) == cannotChangeMemoryG.end() && 
            cannotUseMemoryG.find(address) == cannotUseMemoryG.end();
}

int counter = 1;
void eraseIfP_URG (int reg) {
    auto itr = cannotUseRegistersG.find(reg);
    if(itr != cannotUseRegistersG.end()) cannotUseRegistersG.erase(itr); 
    return;
}
void eraseIfP_CRG (int reg) {
    auto itr = cannotChangeRegistersG.find(reg);
    if(itr != cannotChangeRegistersG.end()) cannotChangeRegistersG.erase(itr); 
    return;
}
void eraseIfP_UMG (int mem) {
    auto itr = cannotUseMemoryG.find(mem);
    if(itr != cannotUseMemoryG.end()) cannotUseMemoryG.erase(itr); 
    return;
}
void eraseIfP_CMG (int mem) {
    auto itr = cannotChangeMemoryG.find(mem);
    if(itr != cannotChangeMemoryG.end()) cannotChangeMemoryG.erase(itr); 
    return;
}

void eraseIfP_LW (instruction curInst) {
    eraseIfP_URG (curInst.cirs[0]);
    eraseIfP_CRG (curInst.cirs[2]);
    eraseIfP_CRG (curInst.cirs[0]);
    eraseIfP_CMG (getAddress(curInst));
    return;
}

void eraseIfP_SW (instruction curInst) {
    eraseIfP_CRG (curInst.cirs[0]);
    eraseIfP_CRG (curInst.cirs[2]);
    eraseIfP_CMG (getAddress(curInst));
    eraseIfP_UMG (getAddress(curInst));
    return;
}

int getAddress (instruction& curInst) {
    cout << "counter at pos 1 :" << counter<<endl;counter++; 
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
    cout << "counter at pos 2 :" << counter<<endl;counter++; 
    return address;
}

void intializerowToInsSet () {
    cout << "counter at pos 3 :" << counter<<endl;counter++; 
    for (auto& x : InstAddToBlocks) {
        cout << "counter at pos 4 :" << counter<<endl;counter++; 
        int instructionAdd = x.first;

       if (InstAddToFreq[instructionAdd] > 0)  {
           cout << "counter at pos 5 :" << counter<<endl;counter++; 
           instruction curInst = iset[instructionAdd-1];
           int address = getAddress(curInst);
            int wantedRow = getRowOfRowBuffer(address);
            cout<<  counter << ": inst address " << instructionAdd << "wanted row " << wantedRow << endl;counter++;
            rowToInsSet[wantedRow].insert(instructionAdd);
       }
    }
    cout << "counter at pos 6 :" << counter<<endl;counter++; 
    return;
}

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

void exeLW (ofstream &out,instruction currInst, int instAddr) {
    cout << "counter at pos 7 :" << counter<<endl;counter++; 
   
    
    ////lwInstRegisterID = currInst.cirs[0];
    int address = getAddress(currInst);
    int prevRow = currentRowInRowBuffer;
    int wantedRow = getRowOfRowBuffer(address);
    if (wantedRow < 0)
    {
        error1 = "BAD ADDRESS (lw)";
    }
    // * start here
    cout << "counter at pos 8 :" << counter<<endl;counter++; 
    cout << "this is numclockcycle in exeLW1:" << numClockCycles<< " insAddr " << instAddr  << endl;
    exeAllBlkingInstLW(out,currInst,2);
    DRAMrequestIssued = true;
    exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (lw)");
    cout << "this is numclockcycle in exeLW2:" << numClockCycles << " insAddr " << instAddr  << endl;
    cout << "counter at pos 9 :" << counter<<endl;counter++; 
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
    cout << "counter at pos 10 :" << counter<<endl;counter++; 
    lw(currInst.cirs);
    cout << "counter at pos 11 :" << counter<<endl;counter++; 
    if (prevRow == -1)
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
    }
    else if (prevRow == wantedRow)
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
    }
    else
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "WriteBack Row " + to_string(prevRow) + " to DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY)) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY) + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
    }
    ////exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1)  + "-" + to_string(uptoClkCyc) + ": " +  rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]) );
    numOfInst[7]++;
    cout << "counter at pos 12 :" << counter<<endl;counter++; 
    
    cout << "this is numclockcycle in exeLW3:" << numClockCycles << " insAddr " << instAddr  << endl;
}

void exeSW (ofstream &out,instruction currInst, int instAddr) {
    cout << "counter at pos 13 :" << counter<<endl;counter++; 
   
    int address = getAddress(currInst);
    int wantedRow = getRowOfRowBuffer(address);
    if (wantedRow < 0)
    {
        error1 = "BAD ADDRESS (sw)";
    }
    int prevRow = currentRowInRowBuffer;
    cout << "counter at pos 14 :" << counter<<endl;counter++; 
    exeAllBlkingInstSW(out,currInst,2);
    DRAMrequestIssued = true;
    exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (sw)");
    cout << "counter at pos 15 :" << counter<<endl;counter++; 
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
    cout << "counter at pos 16 :" << counter<<endl;counter++; 
    sw(currInst.cirs);
    cout << "counter at pos 17 :" << counter<<endl;counter++; 
    numRowBufferUpdates++;

    if (prevRow == -1)
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": memory address " + to_string(address) + "-" + to_string(address + 3) + " = " + to_string(registers[currInst.cirs[0]]));
    }
    else if (prevRow == wantedRow)
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": memory address " + to_string(address) + "-" + to_string(address + 3) + " = " + to_string(registers[currInst.cirs[0]]));
    }
    else
    {
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1) + "-" + to_string(numClockCycles + ROW_ACCESS_DELAY) + ": " + "WriteBack Row " + to_string(prevRow) + " to DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + ROW_ACCESS_DELAY + 1) + "-" + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY)) + ": " + "Access Row " + to_string(wantedRow) + " from DRAM");
        exectutionOutput.push_back("cycle " + to_string(numClockCycles + (2 * ROW_ACCESS_DELAY) + 1) + "-" + to_string(uptoClkCyc) + ": " + "Accessing Column " + to_string(getColOfRowBuffer(address)) + ": memory address " + to_string(address) + "-" + to_string(address + 3) + " = " + to_string(registers[currInst.cirs[0]]));
    }
    numOfInst[8]++;
    cout << "counter at pos 18 :" << counter<<endl;counter++; 
}

void eraseAllBlocks (int insAddr) {
    cout << "counter at pos 19 :" << counter<<endl;counter++; 
    cout<<"Instrution frequncy "<< InstAddToFreq[insAddr] <<endl;
    //if (InstAddToFreq[insAddr] == 1) {
        vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[insAddr];
        // vector[0] = cannotUseRegisters;
        // vector[1] = cannotChangeRegisters;
        // vector[2] = cannotUseMemory;
        // vector[3] = cannotChangeMemory;
        cout << "H1" << endl;
        cout << "inst addressssss" << insAddr << endl;
        //cout << blockedByInsAddr << endl;
        cout << "counter at pos 20 :" << counter<<endl;counter++; 
        for (int reg : blockedByInsAddr[0]) {
            cout << "counter at pos 21 :" << counter<<endl;counter++; 
            cout << "reg " << rrmap[reg] << endl;
            cout << cannotUseRegistersG.size() << "this is size\n";
            auto itr = cannotUseRegistersG.find(reg);
            if (itr != cannotUseRegistersG.end()) cannotUseRegistersG.erase(itr);
            cout << cannotUseRegistersG.size() << "this is size\n";
            ////uBlockRegToInstAdd.erase(reg);
            cout << "counter at pos 22 :" << counter<<endl;counter++; 
        }
        cout << "H2" << endl;
        cout << "counter at pos 23 :" << counter<<endl;counter++; 
        for (int reg : blockedByInsAddr[1]) {
            cout << "counter at pos 24 :" << counter<<endl;counter++; 
            auto itr = cannotChangeRegistersG.find(reg);
            if (itr != cannotChangeRegistersG.end()) cannotChangeRegistersG.erase(itr);
            ////cBlockRegToInstAdd.erase(reg);
        }
        for (int mem : blockedByInsAddr[2]) {
            cout << "counter at pos 25 :" << counter<<endl;counter++; 
            auto itr = cannotUseMemoryG.find(mem);
            if (itr != cannotUseMemoryG.end()) cannotUseMemoryG.erase(itr);
            ////uBlockMemoryToInstAdd.erase(mem);
        }
        //cout << "H4" << endl;
        for (int mem : blockedByInsAddr[3]) {
            cout << "counter at pos 26 :" << counter<<endl;counter++; 
            auto itr = cannotChangeMemoryG.find(mem);
            if (itr != cannotChangeMemoryG.end()) cannotChangeMemoryG.erase(itr);
            ////cBlockMemoryToInstAdd.erase(mem);
        }
        //cout << "H5" << endl;
        //InstAddToBlocks.erase(insAddr);
        //cout << "H6" << endl;
    //}
    InstAddToFreq[insAddr] = max (InstAddToFreq[insAddr]- 1, 0);
    cout << "counter at pos 27 :" << counter<<endl;counter++; 
    return;
}

void removeAndExeOPID7 (ofstream &out,instruction& curInst, int insAddr) {
    cout << "counter at pos 28 :" << counter<<endl;counter++; 
    if (iset[insAddr-1].opID == 7){
        cout << "counter at pos 29 :" << counter<<endl;counter++; 
        eraseIfP_LW(curInst);
        cout << "remove and exe op 7 :: insAddr " << insAddr << " numclockcycle " << numClockCycles << endl;
        cout << "counter at pos 30 :" << counter<<endl;counter++; 
        exeLW (out,iset[insAddr-1],insAddr);
        cout << "counter at pos 31 :" << counter<<endl;counter++; 
        InstAddToFreq[insAddr] = max (InstAddToFreq[insAddr] - 1, 0);
        cout<<"abfasdfadsf "<< insAddr <<endl;
        //eraseAllBlocks(instructionAdd);
        cout << "counter at pos 32 :" << counter<<endl;counter++; 
    } 
    return;
}

void removeAndExeOPID8 (ofstream &out,instruction& curInst, int insAddr) {
    cout << "counter at pos 33 :" << counter<<endl;counter++; 
    if (iset[insAddr-1].opID == 8) {
        cout << "counter at pos 34 :" << counter<<endl;counter++; 
        eraseIfP_SW(curInst);
        cout << "counter at pos 35 :" << counter<<endl;counter++; 
        exeSW (out,iset[insAddr-1],insAddr);
        cout << "counter at pos 36 :" << counter<<endl;counter++; 
        cout<<"abfasdfadf "<< insAddr <<endl;
        InstAddToFreq[insAddr] = max (InstAddToFreq[insAddr]- 1, 0);
        //eraseAllBlocks(instructionAdd); 
        cout << "counter at pos 37 :" << counter<<endl;counter++;                    
    }
}

void exeAllBlkingInstAdd (ofstream &out,instruction currInst , int upto) {
    cout << "counter at pos 38 :" << counter<<endl;counter++; 
    for (int regPos = 0; regPos <= upto; regPos++) {
        cout << "counter at pos 39 :" << counter<<endl;counter++; 
        cout << "register: " << currInst.cirs[regPos] << endl;
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end()) {
            eraseIfP_URG(currInst.cirs[regPos]);
            cout << "counter at pos 40 :" << counter<<endl;counter++; 
            cout << "caught register: " << currInst.cirs[regPos] << endl;
            cout << "opid: " << currInst.opID << endl;
            //cout << exectutionOutput << endl;
            queue<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            cout << "ublkRegQueue size: " << ublkRegQueue.size() << endl;
            while (!ublkRegQueue.empty()) {
                cout << "counter at pos 41 :" << counter<<endl;counter++; 
                int insAddr = ublkRegQueue.front();
                cout << "insAddr inside while loop: " << insAddr << endl;
                cout<< "This is opID" << iset[insAddr-1].opID <<endl;
                cout << "counter at pos 42 :" << counter<<endl;counter++; 
                ublkRegQueue.pop();
                removeAndExeOPID7 (out,currInst,insAddr);
                cout << "counter at pos 43 :" << counter<<endl;counter++; 
                removeAndExeOPID8 (out,currInst,insAddr);
                cout << "counter at pos 44 :" << counter<<endl;counter++; 
                //eraseAllBlocks(insAddr);
                //cout << "hello2\n";

                if (DRAMrequestIssued) {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                cout << "counter at pos 45 :" << counter<<endl;counter++; 
                //ublkRegQueue.pop();
            }
            uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
            cout << "counter at pos 46 :" << counter<<endl;counter++; 
        }
        cout << "counter at pos 47 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 48 :" << counter<<endl;counter++; 
    if (cannotChangeRegistersG.find(currInst.cirs[0]) != cannotChangeRegistersG.end()) {
        eraseIfP_CRG(currInst.cirs[0]);
        cout << "counter at pos 49 :" << counter<<endl;counter++; 
        queue<int> cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];
        while (!cblkRegQueue.empty()) {
            cout << "counter at pos 50 :" << counter<<endl;counter++; 
            int insAddr = cblkRegQueue.front();
            cout << "counter at pos 51 :" << counter<<endl;counter++; 
            removeAndExeOPID7 (out,currInst,insAddr);
            cout << "counter at pos 52 :" << counter<<endl;counter++; 
            removeAndExeOPID8 (out,currInst,insAddr);
            cout << "counter at pos 53 :" << counter<<endl;counter++; 
            //eraseAllBlocks(insAddr);

            if (DRAMrequestIssued) {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cblkRegQueue.pop();
            cout << "counter at pos 54 :" << counter<<endl;counter++; 
        }
        cBlockRegToInstAddQueue[currInst.cirs[0]] = cblkRegQueue;
        cout << "counter at pos 55 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 56 :" << counter<<endl;counter++; 
    return;
}


void exeAllBlkingInstLW (ofstream &out,instruction currInst, int upto) {
    cout << "counter at pos 57 :" << counter<<endl;counter++; 
    eraseIfP_LW(currInst);
    cout << "counter at pos 58 :" << counter<<endl;counter++; 
    for (int regPos = 0; regPos <= upto; regPos += 2) {
        cout << "register: " << currInst.cirs[regPos] << endl;
        cout << "counter at pos 59 :" << counter<<endl;counter++; 

        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end()) {
            eraseIfP_URG(currInst.cirs[regPos]);
            cout << "counter at pos 60 :" << counter<<endl;counter++; 
            cout << "caught register: " << currInst.cirs[regPos] << endl;
            cout << "opid: " << currInst.opID << endl;
            //cout << exectutionOutput << endl;
            queue<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            cout << "ublkRegQueue size: " << ublkRegQueue.size() << endl;
            while (!ublkRegQueue.empty()) {
                cout << "counter at pos 61 :" << counter<<endl;counter++; 
                int insAddr = ublkRegQueue.front();
                for (int x : cannotUseRegistersG) cout << rrmap[x] << " inside cannotUseRegistersG inside lw while before executing " << insAddr <<endl;
                for (int x : cannotChangeRegistersG) cout << rrmap[x] << " inside cannotChangeRegistersG inside lw while before executing " << insAddr <<endl;
                for (int x : cannotUseMemoryG) cout << x << " inside cannotUseMemoryG\n";
                for (int x : cannotChangeMemoryG) cout << x << " inside cannotChangeMemoryG\n";
                cout << "insAddr inside while loop: " << insAddr << endl;
                cout<< "This is opID" << iset[insAddr-1].opID <<endl;
                 cout << "counter at pos 62 :" << counter<<endl;counter++; 
                 ublkRegQueue.pop();
                removeAndExeOPID7 (out,currInst,insAddr);
                 cout << "counter at pos 63 :" << counter<<endl;counter++; 
                removeAndExeOPID8 (out,currInst,insAddr);
                 cout << "counter at pos 64 :" << counter<<endl;counter++; 
                //eraseAllBlocks(insAddr);

                if (DRAMrequestIssued) {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                 cout << "counter at pos 65 :" << counter<<endl;counter++; 
                //ublkRegQueue.pop();
                 cout << "counter at pos 66 :" << counter<<endl;counter++; 
            }
            uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
             cout << "counter at pos 67 :" << counter<<endl;counter++; 
        }
        cout << "counter at pos 68 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 69 :" << counter<<endl;counter++; 
    if (cannotChangeRegistersG.find(currInst.cirs[0]) != cannotChangeRegistersG.end()) {
        eraseIfP_CRG(currInst.cirs[0]);
         cout << "counter at pos 70 :" << counter<<endl;counter++; 
        queue<int> cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];
        while (!cblkRegQueue.empty()) {
            cout << "counter at pos 71 :" << counter<<endl;counter++; 
            int insAddr = cblkRegQueue.front();
            for (int x : cannotUseRegistersG) cout << rrmap[x] << " inside cannotUseRegistersG inside first if while before executing " << insAddr <<endl;
                for (int x : cannotChangeRegistersG) cout << rrmap[x] << " inside cannotChangeRegistersG inside first if while before executing " << insAddr <<endl;
                for (int x : cannotUseMemoryG) cout << x << " inside cannotUseMemoryG\n";
                for (int x : cannotChangeMemoryG) cout << x << " inside cannotChangeMemoryG\n";
            cout << "counter at pos 72 :" << counter<<endl;counter++; 
            removeAndExeOPID7 (out,currInst,insAddr);
            cout << "counter at pos 73 :" << counter<<endl;counter++; 
            removeAndExeOPID8 (out,currInst,insAddr);
            cout << "counter at pos 74 :" << counter<<endl;counter++; 
            //eraseAllBlocks(insAddr);

            if (DRAMrequestIssued) {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cblkRegQueue.pop();
            cout << "counter at pos 75 :" << counter<<endl;counter++; 
        }
        cBlockRegToInstAddQueue[currInst.cirs[0]] = cblkRegQueue;
        cout << "counter at pos 76 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 77 :" << counter<<endl;counter++; 
    if (cannotUseMemoryG.find(getAddress(currInst)) != cannotUseMemoryG.end()) {
        eraseIfP_UMG(getAddress(currInst));
        cout << "counter at pos 78 :" << counter<<endl;counter++; 
        queue<int> ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
        while (! ublkMemQueue.empty()) {
            cout << "counter at pos 79 :" << counter<<endl;counter++; 
            int insAddr = ublkMemQueue.front();
            cout << "counter at pos 80 :" << counter<<endl;counter++; 
            removeAndExeOPID7 (out,currInst,insAddr);
            cout << "counter at pos 81 :" << counter<<endl;counter++; 
            removeAndExeOPID8 (out,currInst,insAddr);
            cout << "counter at pos 82 :" << counter<<endl;counter++; 
            //eraseAllBlocks(insAddr);

            if (DRAMrequestIssued) {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            ublkMemQueue.pop();
            cout << "counter at pos 83 :" << counter<<endl;counter++; 
        }
        uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;
        cout << "counter at pos 84 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 85 :" << counter<<endl;counter++; 
    return;
}



void exeAllBlkingInstSW (ofstream &out,instruction currInst, int upto) {
    cout << "counter at pos 86 :" << counter<<endl;counter++; 
    eraseIfP_SW(currInst);
    cout << "counter at pos 87 :" << counter<<endl;counter++; 
    for (int regPos = 0; regPos <= upto; regPos += 2) {
        cout << "counter at pos 88 :" << counter<<endl;counter++; 
        cout << "register: " << currInst.cirs[regPos] << endl;
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end()) {
            eraseIfP_URG(currInst.cirs[regPos]);
            cout << "counter at pos 89 :" << counter<<endl;counter++; 
            cout << "caught register: " << currInst.cirs[regPos] << endl;
            cout << "opid: " << currInst.opID << endl;
            //cout << exectutionOutput << endl;
            queue<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            cout << "ublkRegQueue size: " << ublkRegQueue.size() << endl;
            while (!ublkRegQueue.empty()) {
                int insAddr = ublkRegQueue.front();
                cout << "counter at pos 90 :" << counter<<endl;counter++; 
                ////cout << "insAddr inside while loop: " << insAddr << endl;
                ////cout<< "This is opID" << iset[insAddr-1].opID <<endl;
                cout << "counter at pos 91 :" << counter<<endl;counter++; 
                ublkRegQueue.pop();
                removeAndExeOPID7 (out,currInst,insAddr);
                cout << "counter at pos 92 :" << counter<<endl;counter++; 
                removeAndExeOPID8 (out,currInst,insAddr);
                cout << "counter at pos 93 :" << counter<<endl;counter++; 
                //eraseAllBlocks(insAddr);
                
                if (DRAMrequestIssued) {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                
                cout << "counter at pos 94 :" << counter<<endl;counter++; 
            }
            uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
            cout << "counter at pos 95 :" << counter<<endl;counter++; 
        }
        cout << "counter at pos 96 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 97 :" << counter<<endl;counter++; 
    if (cannotUseMemoryG.find(getAddress(currInst)) != cannotUseMemoryG.end()) {
        eraseIfP_UMG(getAddress(currInst));
        cout << "counter at pos 98 :" << counter<<endl;counter++; 
        queue<int> ublkMemQueue = uBlockMemoryToInstAddQueue[getAddress(currInst)];
        while (! ublkMemQueue.empty()) {
            cout << "counter at pos 99 :" << counter<<endl;counter++; 
            int insAddr = ublkMemQueue.front();
            cout << "counter at pos 100 :" << counter<<endl;counter++; 
            removeAndExeOPID7 (out,currInst,insAddr);
            cout << "counter at pos 101 :" << counter<<endl;counter++; 
            removeAndExeOPID8 (out,currInst,insAddr);
            cout << "counter at pos 102 :" << counter<<endl;counter++; 
            //eraseAllBlocks(insAddr);

            if (DRAMrequestIssued) {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            ublkMemQueue.pop();
            cout << "counter at pos 103 :" << counter<<endl;counter++; 
        }
        uBlockMemoryToInstAddQueue[getAddress(currInst)] = ublkMemQueue;
        cout << "counter at pos 104 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 105 :" << counter<<endl;counter++; 
    if (cannotChangeMemoryG.find(getAddress(currInst)) != cannotChangeMemoryG.end()) {
        eraseIfP_CMG(getAddress(currInst));
        cout << "counter at pos 106 :" << counter<<endl;counter++; 
        queue<int> cblkMemQueue = cBlockMemoryToInstAddQueue[getAddress(currInst)];
        while (! cblkMemQueue.empty()) {
            cout << "counter at pos 107 :" << counter<<endl;counter++; 
            int insAddr = cblkMemQueue.front();
            cout << "counter at pos 108 :" << counter<<endl;counter++; 
            removeAndExeOPID7 (out,currInst,insAddr);
            cout << "counter at pos 109 :" << counter<<endl;counter++; 
            removeAndExeOPID8 (out,currInst,insAddr);
            cout << "counter at pos 110 :" << counter<<endl;counter++; 
            //eraseAllBlocks(insAddr);

            if (DRAMrequestIssued) {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cblkMemQueue.pop();
            cout << "counter at pos 111 :" << counter<<endl;counter++; 
        }
        cBlockMemoryToInstAddQueue[getAddress(currInst)] = cblkMemQueue;
        cout << "counter at pos 112 :" << counter<<endl;counter++; 
    }
    cout << "counter at pos 113 :" << counter<<endl;counter++; 
    return;
}


void exeAllBlkingInstSub (ofstream &out,instruction currInst, int upto) {
    cout << "counter at pos 114 :" << counter<<endl;counter++; 
    exeAllBlkingInstAdd(out,currInst,upto);
    cout << "counter at pos 115 :" << counter<<endl;counter++; 
    return;
}
void exeAllBlkingInstMul (ofstream &out,instruction currInst,int upto) {
    cout << "counter at pos 116 :" << counter<<endl;counter++; 
    exeAllBlkingInstAdd(out,currInst,upto);
    cout << "counter at pos 117 :" << counter<<endl;counter++; 
    return;
}
void exeAllBlkingInstSlt (ofstream &out,instruction currInst, int upto) {
    cout << "counter at pos 118 :" << counter<<endl;counter++; 
    exeAllBlkingInstAdd(out,currInst,upto);
    cout << "counter at pos 119 :" << counter<<endl;counter++; 
    return;
}

void exeAllBlkingInstAddi (ofstream &out,instruction currInst,int upto) {
    cout << "counter at pos 120 :" << counter<<endl;counter++; 
    exeAllBlkingInstAdd(out,currInst,upto);
    cout << "counter at pos 121 :" << counter<<endl;counter++; 
    return;
}

void exeAllBlkingInstBne (ofstream &out,instruction currInst, int upto) {
    cout << "counter at pos 38 :" << counter<<endl;counter++; 
    for (int regPos = 0; regPos <= upto; regPos++) {
        cout << "counter at pos 39 :" << counter<<endl;counter++; 
        cout << "register: " << currInst.cirs[regPos] << endl;
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end()) {
            eraseIfP_URG(currInst.cirs[regPos]);
            cout << "counter at pos 40 :" << counter<<endl;counter++; 
            cout << "caught register: " << currInst.cirs[regPos] << endl;
            cout << "opid: " << currInst.opID << endl;
            //cout << exectutionOutput << endl;
            queue<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            cout << "ublkRegQueue size: " << ublkRegQueue.size() << endl;
            while (!ublkRegQueue.empty()) {
                cout << "counter at pos 41 :" << counter<<endl;counter++; 
                int insAddr = ublkRegQueue.front();
                cout << "insAddr inside while loop: " << insAddr << endl;
                cout<< "This is opID" << iset[insAddr-1].opID <<endl;
                cout << "counter at pos 42 :" << counter<<endl;counter++; 
                ublkRegQueue.pop();
                removeAndExeOPID7 (out,currInst,insAddr);
                cout << "counter at pos 43 :" << counter<<endl;counter++; 
                removeAndExeOPID8 (out,currInst,insAddr);
                cout << "counter at pos 44 :" << counter<<endl;counter++; 
                //eraseAllBlocks(insAddr);
                //cout << "hello2\n";

                if (DRAMrequestIssued) {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                cout << "counter at pos 45 :" << counter<<endl;counter++; 
                //ublkRegQueue.pop();
            }
            uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
            cout << "counter at pos 46 :" << counter<<endl;counter++; 
        }
        cout << "counter at pos 47 :" << counter<<endl;counter++; 
    }
    
    return;
}
void exeAllBlkingInstBeq (ofstream &out,instruction currInst,int upto) {
    cout << "counter at pos 132 :" << counter<<endl;counter++; 
    exeAllBlkingInstBne(out,currInst,upto);
    cout << "counter at pos 133 :" << counter<<endl;counter++; 
    return;
}




void execute1a4(ofstream &out)
{
    cout << "counter at pos 135 :" << counter<<endl;counter++; 
    while (pc <= maxInstructions)
    {
        cout << "counter at pos 136 :" << counter<<endl;counter++; 
        if (error1 != "")
        {
            cout << "counter at pos 137 :" << counter<<endl;counter++; 
            cout << "Runtime Error:" << pc - 1 << ":" << error1 << endl;
            return;
        }
        cout << "counter at pos 138 :" << counter<<endl;counter++; 
        numClockCycles++;
        if (numClockCycles > uptoClkCyc)
        {
            cout << "counter at pos 139 :" << counter<<endl;counter++; 
            DRAMrequestIssued = false;
        }
        cout << "counter at pos 140 :" << counter<<endl;counter++; 
        instruction currInst = iset[pc - 1];
        pc++;

        switch (currInst.opID)
        {
        case 0:
        { // *add
            cout << "counter at pos 141 :" << counter<<endl;counter++; 
            if (DRAMrequestIssued) 
            { 
                cout << "counter at pos 142 :" << counter<<endl;counter++; 
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; 
                ////lwInstRegisterID = -1;
                ////swInstMemAdd = -1;
                ////blockRegisterID = -1;
                ////blockMemoryAdd = -1;
            }
            cout << "counter at pos 143 :" << counter<<endl;counter++; 
            cout << "numclock cycleeeee1: " << numClockCycles<< endl;
            exeAllBlkingInstAdd(out,currInst,2);
            cout << "counter at pos 144 :" << counter<<endl;counter++;  
            cout << "numclock cycleeeee2: " << numClockCycles<< endl;
            add(currInst.cirs);
            cout << "numclock cycleeeee3: " << numClockCycles<< endl;
            cout << "counter at pos 145 :" << counter<<endl;counter++; 
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: add (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[0]++;

             break;   
        }
        case 1:
        { // *sub
            cout << "counter at pos 146 :" << counter<<endl;counter++; 
            if (DRAMrequestIssued) 
            { 
                cout << "counter at pos 147 :" << counter<<endl;counter++; 
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 148 :" << counter<<endl;counter++; 
            exeAllBlkingInstSub(out,currInst,2);
            cout << "counter at pos 149 :" << counter<<endl;counter++; 
            sub(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: sub (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[1]++;
            cout << "counter at pos 150 :" << counter<<endl;counter++; 
            break;
        }
        case 2:
        { // *mul
            cout << "counter at pos 151 :" << counter<<endl;counter++;
            if (DRAMrequestIssued) 
            { 
                cout << "counter at pos 152 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; 
            }
            cout << "counter at pos 153 :" << counter<<endl;counter++;
            exeAllBlkingInstMul(out,currInst,2);
            cout << "counter at pos 154 :" << counter<<endl;counter++;
            mul(currInst.cirs);
            cout << "counter at pos 155 :" << counter<<endl;counter++;
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: Mul (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[2]++;

            break;
        }
        case 3:
        { // *slt
            cout << "counter at pos 156 :" << counter<<endl;counter++;
            if (DRAMrequestIssued)
            {
                cout << "counter at pos 157 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 158 :" << counter<<endl;counter++;
            exeAllBlkingInstSlt (out,currInst,2);
            cout << "counter at pos 159 :" << counter<<endl;counter++;
            slt(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: slt (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[3]++;
            cout << "counter at pos 160 :" << counter<<endl;counter++;
            break;
        }

        case 4:
        { // *addi
            cout << "counter at pos 161 :" << counter<<endl;counter++;
            if (DRAMrequestIssued)
            {
                cout << "counter at pos 162 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 163 :" << counter<<endl;counter++;
            exeAllBlkingInstAddi(out,currInst,1);
            cout << "counter at pos 164 :" << counter<<endl;counter++;
            addi(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: addi (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + to_string(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[4]++;
            cout << "counter at pos 165 :" << counter<<endl;counter++;
            break;
        }

        // *NOTE THAT WHENEVER YOU ARE BRANCHING, YOU HAVE TO WAIT IF THE REGISTERS ARE NOT AVAILABLE BECAUSE YOU CANNOT DECIDE WHICH INSTRUCTION TO EXECUTE FIRST
        // *HENCE WAIT UNTILL ALL CLOCK CYCLES TO LOAD THE NECESSARY REGISTER AND THEN EXECUTE THE CURRENT COMMAND.
        case 5:
        { // *bne
            cout << "counter at pos 166 :" << counter<<endl;counter++;
            if (DRAMrequestIssued)
            {
                cout << "counter at pos 167 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 168 :" << counter<<endl;counter++;
            exeAllBlkingInstBne (out,currInst,1);
            cout << "counter at pos 169 :" << counter<<endl;counter++;
            bne(currInst.cirs, currInst.label);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction bne (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + currInst.label + "," + to_string(switchOnBranch) + ")");
            numOfInst[5]++;
            cout << "counter at pos 170 :" << counter<<endl;counter++;
            break;
        }
        case 6:
        { // *beq
            cout << "counter at pos 171 :" << counter<<endl;counter++;
            if (DRAMrequestIssued)
            {
                cout << "counter at pos 172 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 173 :" << counter<<endl;counter++;
            exeAllBlkingInstBeq (out,currInst,1);
            cout << "counter at pos 174 :" << counter<<endl;counter++;
            beq(currInst.cirs, currInst.label);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction beq (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + currInst.label + "," + to_string(switchOnBranch) + ")");
            numOfInst[6]++;
            cout << "counter at pos 175 :" << counter<<endl;counter++;
            break;
        }
        //lw, sw := [register1, offset , last register , 0] // 0 if register is there as last arguement inside parenthesis
        // := [register1, offset ,address , 1]  // 1 if address given as argument inside braces
        case 7:
        { //lw
            cout << "counter at pos 176 :" << counter<<endl;counter++;
            if (DRAMrequestIssued)
            {
                cout << "counter at pos 177 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            //exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (lw)");
            //DRAMrequestIssued = true;
            //lwInstRegisterID = currInst.cirs[0];
            cout << "counter at pos 178 :" << counter<<endl;counter++;
            int address = getAddress(currInst);
            int prevRow = currentRowInRowBuffer;
            int wantedRow = getRowOfRowBuffer(address);
            // * Now we can see if we want to execute it now or not
            
            if (wantedRow < 0)
                error1 = "BAD ADDRESS (lw)";
            else if (currentRowInRowBuffer == -1 || (currentRowInRowBuffer == wantedRow && 
                    ifLW(currInst,address)))
            {   // * Here we should execute immediately.
                cout << "counter at pos 179 :" << counter<<endl;counter++;
                exeLW(out,currInst,pc-1);
            }
            else // * even if row buffer is same, if some registers are blocked, then we will need to execute 
                // * the blocked instructions first, which may change the row buffer, hence do not execute if some are blocked.
            {   //* We do not need to execute it currently. postpone.
                numClockCycles--;
                cout << "counter at pos 180 :" << counter<<endl;counter++;
                //cout << "hello\n";
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
                
                vector<unordered_set<int>> temp (4);
                cout << "counter at pos 181 :" << counter<<endl;counter++;
                temp[0] = cannotUseRegisters;
                temp[1] = cannotChangeRegisters;
                temp[2] = cannotUseMemory;
                temp[3] = cannotChangeMemory;
                //cout << "Hello2\n";
                InstAddToBlocks[pc-1] = temp;
                InstAddToFreq[pc-1]++;
                cout << "first register " << currInst.cirs[0] <<" instruction frequency lw" <<  InstAddToFreq[pc-1] << " pc ="<< pc<<  endl;
                uBlockRegToInstAddQueue[currInst.cirs[0]].push(pc-1);
                cBlockRegToInstAddQueue[currInst.cirs[0]].push(pc-1);
                cBlockRegToInstAddQueue[currInst.cirs[2]].push(pc-1);
                cBlockMemoryToInstAddQueue[address].push(pc-1);
                cout << "counter at pos 182 :" << counter<<endl;counter++;
            }    
            cout << "counter at pos 183 :" << counter<<endl;counter++;
            break;
        }

        case 8:
        { //sw
            cout << "counter at pos 184 :" << counter<<endl;counter++;
            if (DRAMrequestIssued)
            {
                cout << "counter at pos 185 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 186:" << counter<<endl;counter++;
            
            int address = getAddress(currInst);
            int wantedRow = getRowOfRowBuffer(address);
            int prevRow = currentRowInRowBuffer;
            if (wantedRow < 0)
                error1 = "BAD ADDRESS (sw)";
            else if (currentRowInRowBuffer == -1 ||(currentRowInRowBuffer == wantedRow && 
                    ifSW(currInst,address)))
                    
            { // * can execute similar to lw case
                cout << "counter at pos 187 :" << counter<<endl;counter++;
                exeSW(out,currInst,pc-1);
            }
            
            else
            {   // * postpone similar to lw case
            cout << "counter at pos 188 :" << counter<<endl;counter++;
                numClockCycles--;
                cout << "hee" << endl;
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
    
                vector<unordered_set<int>> temp (4);
                cout << "counter at pos 189 :" << counter<<endl;counter++;
                temp[0] = cannotUseRegisters;
                temp[1] = cannotChangeRegisters;
                temp[2] = cannotUseMemory;
                temp[3] = cannotChangeMemory;
                cout << temp[0].size() << " "<< temp[1].size() << " "<< temp[2].size() << " "<< temp[3].size() << "\n";
                InstAddToBlocks[pc-1] = temp;
                InstAddToFreq[pc-1]++;
                cout << "first register " << currInst.cirs[0] <<" instruction frequency sw" <<  InstAddToFreq[pc-1] << " pc ="<< pc<<  endl;
                cBlockRegToInstAddQueue[currInst.cirs[0]].push(pc-1);
                cBlockRegToInstAddQueue[currInst.cirs[2]].push(pc-1);
                cBlockMemoryToInstAddQueue[address].push(pc-1);
                uBlockMemoryToInstAddQueue[address].push(pc-1);
                cout << "counter at pos 190 :" << counter<<endl;counter++;
            }
            cout << "counter at pos 191 :" << counter<<endl;counter++;
            break;
        }

        case 9:
        { //j       // note that here j instruction needs to wait before the DRAM calls are finished.
            cout << "counter at pos 192 :" << counter<<endl;counter++;
            if (DRAMrequestIssued)
            {
                cout << "counter at pos 193 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 194 :" << counter<<endl;counter++;
            j(currInst.cirs, currInst.label);
            numOfInst[9]++;
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction j :" + "Jump to label ID: " + currInst.label);
            
            break;
        }
        };
        if (registers[0] != 0) error1 = "Zero register cannot be modified";
    }


    numClockCycles++;


    cout << "counter at pos 195 :" << counter<<endl;counter++;
    for (auto & x : InstAddToBlocks) {
       int instructionAdd = x.first;
        cout << "counter at pos 196 :" << counter<<endl;counter++;
        if (InstAddToFreq[instructionAdd] > 0)  {
            cout << "counter at pos 197 :" << counter<<endl;counter++;
            instruction curInst = iset[instructionAdd-1];
            int address = getAddress (curInst);
            int wantedRow = getRowOfRowBuffer(address);
            cout << "currentRowInRowBuffer " << currentRowInRowBuffer << " " << "wantedRow " << wantedRow << endl;
            if (currentRowInRowBuffer == wantedRow) {
                bool done = true;
                cout << "counter at pos 198 :" << counter<<endl;counter++;
                while (done && InstAddToFreq[instructionAdd] > 0) {
                    cout << "Haha" << " " << instructionAdd << endl;
                    cout << "counter at pos 199 :" << counter<<endl;counter++;
                    if (iset[instructionAdd-1].opID == 7){
                        cout << "counter at pos 200 :" << counter<<endl;counter++;
                        eraseIfP_LW(curInst);

                        if(ifLW (curInst,address)) 
                        {
                            cout << "counter at pos 201 :" << counter<<endl;counter++;
                            exeLW (out,iset[instructionAdd-1],instructionAdd);
                            InstAddToFreq[instructionAdd]--;
                            cout<<"abfasdfadsf "<< instructionAdd <<endl;
                            //eraseAllBlocks(instructionAdd);

                        } 
                        cout << "counter at pos 202 :" << counter<<endl;counter++;
                    }
                    
                    else if (iset[instructionAdd-1].opID == 8) {
                        cout << "counter at pos 203 :" << counter<<endl;counter++;
                        eraseIfP_SW(curInst);

                        if (ifSW(curInst,address))
                        {
                            cout << "counter at pos 204 :" << counter<<endl;counter++;
                            exeSW (out,iset[instructionAdd-1],instructionAdd);
                            cout<<"abfasdfadf "<< instructionAdd <<endl;
                            InstAddToFreq[instructionAdd]--;
                            //eraseAllBlocks(instructionAdd);
                        }
                        cout << "counter at pos 205 :" << counter<<endl;counter++;
                    } 
                    else done = false;
                    cout << "counter at pos 206 :" << counter<<endl;counter++;
                    if (InstAddToFreq[instructionAdd] == 0) {
                        cout << "counter at pos 207 :" << counter<<endl;counter++;
                        vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[instructionAdd];

                        for (int reg : blockedByInsAddr[0]) cannotUseRegistersG.erase(reg);
                        for (int reg : blockedByInsAddr[1]) cannotChangeRegistersG.erase(reg);
                        for (int mem : blockedByInsAddr[2]) cannotUseMemoryG.erase(mem);
                        for (int mem : blockedByInsAddr[3]) cannotChangeMemoryG.erase(mem);
                    }
                    //InstAddToFreq[instructionAdd]--;
                    
                    cout << "counter at pos 208 :" << counter<<endl;counter++;
                    if (DRAMrequestIssued) {
                        cout << "counter at pos 209 :" << counter<<endl;counter++;
                        numClockCycles = uptoClkCyc + 1;
                        DRAMrequestIssued = false;
                    }
                }
                cout << "counter at pos 210 :" << counter<<endl;counter++;
            }
            cout << "counter at pos 211 :" << counter<<endl;counter++;
       }
       cout << "counter at pos 212 :" << counter<<endl;counter++;
       
    }
    cout << "counter at pos 213 :" << counter<<endl;counter++;
    intializerowToInsSet();
    cout << "counter at pos 214 :" << counter<<endl;counter++;
    for (auto & x :rowToInsSet) {
        cout << "counter at pos 215 :" << counter<<endl;counter++;
        int rownum = x.first;
        for (auto & instructionAdd : rowToInsSet[rownum]) {
            cout << "counter at pos 216 :" << counter<<endl;counter++;
            instruction curInst = iset[instructionAdd-1];
            int address = getAddress(curInst);
            cout<< "inst address " << instructionAdd << " row num " <<rownum << endl;
            bool done = true;
            while (done && InstAddToFreq[instructionAdd] > 0) {
                cout << "counter at pos 217 :" << counter<<endl;counter++;
                for (int x : cannotUseRegistersG) cout << rrmap[x] << " inside cannotUseRegistersG\n";
                for (int x : cannotChangeRegistersG) cout << rrmap[x] << " inside cannotChangeRegistersG\n";
                for (int x : cannotUseMemoryG) cout << x << " inside cannotUseMemoryG\n";
                for (int x : cannotChangeMemoryG) cout << x << " inside cannotChangeMemoryG\n";
                cout << "Haha" << " " << instructionAdd << endl;
                cout << "counter at pos 218 :" << counter<<endl;counter++;
                if (iset[instructionAdd-1].opID == 7){
                    cout << "counter at pos 219 :" << counter<<endl;counter++;
                    eraseIfP_LW(curInst);
                    cout << "counter at pos 220 :" << counter<<endl;counter++;
                    if(ifLW(curInst,address)) 
                    {
                        cout << "counter at pos 221 :" << counter<<endl;counter++;
                        exeLW (out,iset[instructionAdd-1],instructionAdd);
                        InstAddToFreq[instructionAdd]--;
                        cout<<"abfasdfadsf "<< instructionAdd <<endl;
                        //eraseAllBlocks(instructionAdd);

                    } 
                    cout << "counter at pos 222 :" << counter<<endl;counter++;
                }
                
                else if (iset[instructionAdd-1].opID == 8) {
                    cout << "counter at pos 223 :" << counter<<endl;counter++;
                    eraseIfP_SW(curInst);
                    cout << "counter at pos 224 :" << counter<<endl;counter++;
                    if (ifSW(curInst,address))
                    {
                        cout << "counter at pos 225 :" << counter<<endl;counter++;
                        exeSW (out,iset[instructionAdd-1],instructionAdd);
                        cout<<"abfasdfadf "<< instructionAdd <<endl;
                        InstAddToFreq[instructionAdd]--;
                        //eraseAllBlocks(instructionAdd);
                    }
                    cout << "counter at pos 226 :" << counter<<endl;counter++;
                }
                else done = false;
                cout << "counter at pos 227 :" << counter<<endl;counter++;
                if (InstAddToFreq[instructionAdd] == 0) {
                    cout << "counter at pos 228 :" << counter<<endl;counter++;
                    vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[instructionAdd];

                    for (int reg : blockedByInsAddr[0]) cannotUseRegistersG.erase(reg);
                    for (int reg : blockedByInsAddr[1]) cannotChangeRegistersG.erase(reg);
                    for (int mem : blockedByInsAddr[2]) cannotUseMemoryG.erase(mem);
                    for (int mem : blockedByInsAddr[3]) cannotChangeMemoryG.erase(mem);
                }
                //InstAddToFreq[instructionAdd]--;
                cout << "counter at pos 229 :" << counter<<endl;counter++;
                if (DRAMrequestIssued) {
                    cout << "counter at pos 230 :" << counter<<endl;counter++;
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                cout << "counter at pos 231 :" << counter<<endl;counter++;
            }
            cout << "counter at pos 232 :" << counter<<endl;counter++;
        }
        cout << "counter at pos 233 :" << counter<<endl;counter++;
    }
    cout << "counter at pos 234 :" << counter<<endl;counter++;

    // * FINAL 
    for (auto& x : InstAddToBlocks) {
        cout << "counter at pos 235 :" << counter<<endl;counter++;
        int instructionAdd = x.first;
        
        while (InstAddToFreq[instructionAdd] > 0) {
            cout << "counter at pos 236 :" << counter<<endl;counter++;
            if (iset[instructionAdd-1].opID == 7) exeLW (out,iset[instructionAdd-1],instructionAdd);
            cout << "counter at pos 237 :" << counter<<endl;counter++;
            if (iset[instructionAdd-1].opID == 8) exeSW (out,iset[instructionAdd-1],instructionAdd);
            cout << "counter at pos 238 :" << counter<<endl;counter++;
            if (InstAddToFreq[instructionAdd] == 1) {
                cout << "counter at pos 239 :" << counter<<endl;counter++;
                vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[instructionAdd];

                for (int reg : blockedByInsAddr[0]) cannotUseRegistersG.erase(reg);
                for (int reg : blockedByInsAddr[1]) cannotChangeRegistersG.erase(reg);
                for (int mem : blockedByInsAddr[2]) cannotUseMemoryG.erase(mem);
                for (int mem : blockedByInsAddr[3]) cannotChangeMemoryG.erase(mem);
            }
            cout << "counter at pos 240 :" << counter<<endl;counter++;
            InstAddToFreq[instructionAdd]--;
            
            if (DRAMrequestIssued) {
                cout << "counter at pos 241 :" << counter<<endl;counter++;
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cout << "counter at pos 242 :" << counter<<endl;counter++;
        }
        cout << "counter at pos 243 :" << counter<<endl;counter++;
    }
    numClockCycles--;
    cout << "counter at pos 244 :" << counter<<endl;counter++;
    if (!(cannotUseMemoryG.empty() && cannotChangeMemoryG.empty() &&
        cannotUseRegistersG.empty() && cannotChangeRegistersG.empty())) 
    {
            cout << "counter at pos 245 :" << counter<<endl;counter++;
            cout << "RUNTIME WARNING: All Registers and Memory Addresses should be free by now but some are busy\n";
    }
    if (error1 != "")
    {
        cout << "counter at pos 246 :" << counter<<endl;counter++;
        cout << "Runtime Error:" << pc << ":" << error1 << endl;
        return;
    }
    cout << "counter at pos 247 END OF EXE :" << counter<<endl;counter++;
    return;
}