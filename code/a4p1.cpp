#include "a4p1.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////// THIS IS FOR PART 1 OF THE ASSIGNMENT ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

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
void exeLW (ofstream &out,instruction currInst) {
    exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (lw)");
    DRAMrequestIssued = true;
    ////lwInstRegisterID = currInst.cirs[0];
    int offset = currInst.cirs[1];
    int typeOfarg = currInst.cirs[3];
    int address;
    if (typeOfarg == 0)
    {
        address = offset + registers[currInst.cirs[2]];
        ////blockRegisterID = currInst.cirs[2];
    }
    else
    {
        address = offset + currInst.cirs[2];
        blockMemoryAdd = address;
    }

    int prevRow = currentRowInRowBuffer;
    int wantedRow = getRowOfRowBuffer(address);
    if (wantedRow < 0)
    {
        error1 = "BAD ADDRESS (lw)";
    }
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
}

void exeSW (ofstream &out,instruction currInst) {
    exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (sw)");
    DRAMrequestIssued = true;

    int offset = currInst.cirs[1];
    int typeOfarg = currInst.cirs[3];
    int address;
    //if(typeOfarg == 0) address=offset + registers[currInst.cirs[2]];
    //else address = offset + currInst.cirs[2];
    lwInstRegisterID = currInst.cirs[0];
    if (typeOfarg == 0)
    {
        address = offset + registers[currInst.cirs[2]];
        blockRegisterID = currInst.cirs[2];
    }
    else
    {
        address = offset + currInst.cirs[2];
        blockMemoryAdd = address;
    }
    swInstMemAdd = address;

    int wantedRow = getRowOfRowBuffer(address);
    if (wantedRow < 0)
    {
        error1 = "BAD ADDRESS (sw)";
    }
    int prevRow = currentRowInRowBuffer;
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
    //exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1)  + "-" + to_string(uptoClkCyc) + ": " + "memory address " + to_string(address) + "-" + to_string(address + 3) + " = " + to_string(registers[currInst.cirs[0]])  );
    numOfInst[8]++;
}

void eraseAllBlocks (int insAddr) {
    vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[insAddr];
    // vector[0] = cannotUseRegisters;
    // vector[1] = cannotChangeRegisters;
    // vector[2] = cannotUseMemory;
    // vector[3] = cannotChangeMemory;
    for (int reg : blockedByInsAddr[0]) {
        cannotUseRegistersG.erase(reg);
        ////uBlockRegToInstAdd.erase(reg);
    }
    for (int reg : blockedByInsAddr[1]) {
        cannotChangeRegistersG.erase(reg);
        ////cBlockRegToInstAdd.erase(reg);
    }
    for (int mem : blockedByInsAddr[2]) {
        cannotUseMemoryG.erase(mem);
        ////uBlockMemoryToInstAdd.erase(mem);
    }
    for (int mem : blockedByInsAddr[3]) {
        cannotChangeMemoryG.erase(mem);
        ////cBlockMemoryToInstAdd.erase(mem);
    }
    InstAddToBlocks.erase(insAddr);
    return;
}

void exeAllBlkingInstAdd (ofstream &out,instruction currInst , int upto) {
    for (int regPos = 0; regPos <= upto; regPos++) {
        cout << "regPos: " << currInst.cirs[regPos] << endl;
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end()) {
            cout << "caught regPos: " << currInst.cirs[regPos] << endl;
            queue<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            while (!ublkRegQueue.empty()) {
                int insAddr = ublkRegQueue.front();
                if (iset[insAddr].opID == 7) exeLW (out,iset[insAddr]);
                else if (iset[insAddr].opID == 8) exeSW (out,iset[insAddr]);
                eraseAllBlocks(insAddr);

                if (DRAMrequestIssued) {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                ublkRegQueue.pop();
            }
            uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
        }
    }
    if (cannotChangeRegistersG.find(currInst.cirs[0]) != cannotChangeRegistersG.end()) {
        queue<int> cblkRegQueue = cBlockRegToInstAddQueue[currInst.cirs[0]];
        while (!cblkRegQueue.empty()) {
            int insAddr = cblkRegQueue.front();
            if (iset[insAddr].opID == 7) exeLW (out,iset[insAddr]);
            else if (iset[insAddr].opID == 8) exeSW (out,iset[insAddr]);
            eraseAllBlocks(insAddr);

            if (DRAMrequestIssued) {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            cblkRegQueue.pop();
        }
        cBlockRegToInstAddQueue[currInst.cirs[0]] = cblkRegQueue;
    }
    return;
}
void exeAllBlkingInstSub (ofstream &out,instruction currInst, int upto) {
    exeAllBlkingInstAdd(out,currInst,upto);
    return;
}
void exeAllBlkingInstMul (ofstream &out,instruction currInst,int upto) {
    exeAllBlkingInstAdd(out,currInst,upto);
    return;
}
void exeAllBlkingInstSlt (ofstream &out,instruction currInst, int upto) {
    exeAllBlkingInstAdd(out,currInst,upto);
    return;
}

void exeAllBlkingInstAddi (ofstream &out,instruction currInst,int upto) {
    exeAllBlkingInstAdd(out,currInst,upto);
    return;
}

void exeAllBlkingInstBne (ofstream &out,instruction currInst, int upto) {
    for (int regPos = 0; regPos <= upto; regPos++) {
        if (cannotUseRegistersG.find(currInst.cirs[regPos]) != cannotUseRegistersG.end()) {
            queue<int> ublkRegQueue = uBlockRegToInstAddQueue[currInst.cirs[regPos]];
            while (!ublkRegQueue.empty()) {
                int insAddr = ublkRegQueue.front();
                if (iset[insAddr].opID == 7) exeLW (out,iset[insAddr]);
                else if (iset[insAddr].opID == 8) exeSW (out,iset[insAddr]);
                eraseAllBlocks(insAddr);

                if (DRAMrequestIssued) {
                    numClockCycles = uptoClkCyc + 1;
                    DRAMrequestIssued = false;
                }
                ublkRegQueue.pop();
            }
            uBlockRegToInstAddQueue[currInst.cirs[regPos]] = ublkRegQueue;
        }
    }
    return;
}
void exeAllBlkingInstBeq (ofstream &out,instruction currInst,int upto) {
    exeAllBlkingInstBne(out,currInst,upto);
    return;
}




void execute1a4(ofstream &out)
{
    while (pc <= maxInstructions)
    {
        if (error1 != "")
        {
            cout << "Runtime Error:" << pc - 1 << ":" << error1 << endl;
            return;
        }

        numClockCycles++;
        if (numClockCycles > uptoClkCyc)
        {
            DRAMrequestIssued = false;
            lwInstRegisterID = -1;
            swInstMemAdd = -1;
            blockRegisterID = -1;
            blockMemoryAdd = -1;
        }
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
                ////lwInstRegisterID = -1;
                ////swInstMemAdd = -1;
                ////blockRegisterID = -1;
                ////blockMemoryAdd = -1;
            }

            exeAllBlkingInstAdd(out,currInst,2); 
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

            exeAllBlkingInstSub(out,currInst,2);
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

            exeAllBlkingInstMul(out,currInst,2);
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
            exeAllBlkingInstSlt (out,currInst,2);
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
            exeAllBlkingInstAddi(out,currInst,1);
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
            exeAllBlkingInstBne (out,currInst,1);
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
            exeAllBlkingInstBeq (out,currInst,1);
            beq(currInst.cirs, currInst.label);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction beq (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + currInst.label + "," + to_string(switchOnBranch) + ")");
            numOfInst[6]++;

            break;
        }
        //lw, sw := [register1, offset , last register , 0] // 0 if register is there as last arguement inside parenthesis
        // := [register1, offset ,address , 1]  // 1 if address given as argument inside braces
        case 7:
        { //lw
            if (DRAMrequestIssued)
            {
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false;
            }
            //exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (lw)");
            //DRAMrequestIssued = true;
            //lwInstRegisterID = currInst.cirs[0];

            int offset = currInst.cirs[1];
            int typeOfarg = currInst.cirs[3];
            int address;
            if (typeOfarg == 0)
            {
                address = offset + registers[currInst.cirs[2]];
                blockRegisterID = currInst.cirs[2];
            }
            else
            {
                address = offset + currInst.cirs[2];
                blockMemoryAdd = address;
            }

            int prevRow = currentRowInRowBuffer;
            int wantedRow = getRowOfRowBuffer(address);
            // * Now we can see if we want to execute it now or not
            
            if (wantedRow < 0)
                error1 = "BAD ADDRESS (lw)";
            else if (currentRowInRowBuffer == -1 || (currentRowInRowBuffer == wantedRow && 
                    cannotChangeRegistersG.find (currInst.cirs[0]) == cannotChangeRegistersG.end() &&
                    cannotUseRegistersG.find (currInst.cirs[0]) == cannotUseRegistersG.end() && 
                    cannotUseRegistersG.find (currInst.cirs[2]) == cannotUseRegistersG.end() && 
                    cannotUseMemoryG.find(address) == cannotUseMemoryG.end()))
            {   // * Here we should execute immediately.

                exeLW(out,currInst);
            }
            else // * even if row buffer is same, if some registers are blocked, then we will need to execute 
                // * the blocked instructions first, which may change the row buffer, hence do not execute if some are blocked.
            {   //* We do not need to execute it currently. postpone.
                numClockCycles--;
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

                temp[0] = cannotUseRegisters;
                temp[1] = cannotChangeRegisters;
                temp[2] = cannotUseMemory;
                temp[3] = cannotChangeMemory;
                //cout << "Hello2\n";
                InstAddToBlocks[pc] = temp;
                uBlockRegToInstAddQueue[currInst.cirs[0]].push(pc);
                cBlockRegToInstAddQueue[currInst.cirs[0]].push(pc);
                cBlockRegToInstAddQueue[currInst.cirs[2]].push(pc);
                cBlockMemoryToInstAddQueue[address].push(pc);
                //cout << "hello3\n";
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

            //exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (sw)");
            //DRAMrequestIssued = true;
            int offset = currInst.cirs[1];
            int typeOfarg = currInst.cirs[3];
            int address;
            //if(typeOfarg == 0) address=offset + registers[currInst.cirs[2]];
            //else address = offset + currInst.cirs[2];
            //lwInstRegisterID = currInst.cirs[0];

            if (typeOfarg == 0)
            {
                address = offset + registers[currInst.cirs[2]];
                blockRegisterID = currInst.cirs[2];
            }
            else
            {
                address = offset + currInst.cirs[2];
                blockMemoryAdd = address;
            }
            //swInstMemAdd = address;

            int wantedRow = getRowOfRowBuffer(address);
            int prevRow = currentRowInRowBuffer;
            if (wantedRow < 0)
                error1 = "BAD ADDRESS (sw)";
            else if (currentRowInRowBuffer == -1 ||(currentRowInRowBuffer == wantedRow && 
                    cannotUseRegistersG.find (currInst.cirs[0]) == cannotUseRegistersG.end() && 
                    cannotUseRegistersG.find (currInst.cirs[2]) == cannotUseRegistersG.end() &&
                    cannotChangeMemoryG.find(address) == cannotChangeMemoryG.end()))
            { // * can execute similar to lw case
                exeSW(out,currInst);
            }
                
            else
            {   // * postpone similar to lw case
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
                
                vector<unordered_set<int>> temp (4);

                temp[0] = cannotUseRegisters;
                temp[1] = cannotChangeRegisters;
                temp[2] = cannotUseMemory;
                temp[3] = cannotChangeMemory;

                InstAddToBlocks[pc] = temp;
                cBlockRegToInstAddQueue[currInst.cirs[0]].push(pc);
                cBlockRegToInstAddQueue[currInst.cirs[2]].push(pc);
                cBlockMemoryToInstAddQueue[address].push(pc);
                uBlockMemoryToInstAddQueue[address].push(pc);
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
    }
    //cout << "Hello4\n";
/*
    for (auto itr = InstAddToBlocks.begin(); itr != InstAddToBlocks.end(); itr++) {
        int instructionAdd = itr->first;
        if (iset[instructionAdd].opID == 7) exeLW (out,iset[instructionAdd]);
        else if (iset[instructionAdd].opID == 8) exeSW (out,iset[instructionAdd]);
        eraseAllBlocks(instructionAdd);
        if (DRAMrequestIssued) {
            numClockCycles = uptoClkCyc + 1;
            DRAMrequestIssued = false;
        }
    }
    */
    //int counterr = 0;
    for (auto& x : InstAddToBlocks) {
        //counterr++;
        //cout << "Haha" << counterr << endl;
        int instructionAdd = x.first;
        //cout << "Haha" << counterr << " " << instructionAdd << endl;
        if (iset[instructionAdd].opID == 7) exeLW (out,iset[instructionAdd]);
        //cout << "Yes1\n";
        if (iset[instructionAdd].opID == 8) exeSW (out,iset[instructionAdd]);
        //cout << "Yes2\n";

        vector<unordered_set<int>> blockedByInsAddr = InstAddToBlocks[instructionAdd];
        for (int reg : blockedByInsAddr[0]) cannotUseRegistersG.erase(reg);
        for (int reg : blockedByInsAddr[1]) cannotChangeRegistersG.erase(reg);
        for (int mem : blockedByInsAddr[2]) cannotUseMemoryG.erase(mem);
        for (int mem : blockedByInsAddr[3]) cannotChangeMemoryG.erase(mem);
        //cout << "Yes3\n";
        if (DRAMrequestIssued) {
            numClockCycles = uptoClkCyc + 1;
            DRAMrequestIssued = false;
        }
        //cout << "Yes4\n";
    }
    //cout << "hello5\n";
    if (!(cannotUseMemoryG.empty() && cannotChangeMemoryG.empty() &&
        cannotUseRegistersG.empty() && cannotChangeRegistersG.empty())) {
            cout << "Unknown Error: All Registers and Memory Addresses should be free by now but some are busy\n";
        }
    if (error1 != "")
    {
        cout << "Runtime Error:" << pc << ":" << error1 << endl;
        return;
    }
    return;
}