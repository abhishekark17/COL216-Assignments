#include "executeP1.hpp"

///////////////////////////////////////////////////////////////////////////////////////////
////////////////// THIS IS FOR PART 1 OF THE ASSIGNMENT ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

void execute1(ofstream &out)
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
        { //add
            if ((DRAMrequestIssued == false))
            {
            }
            // DRAMrequestIssued = true now
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }

            add(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: add (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[0]++;

            break;
        }

        case 1:
        { //sub
            if ((DRAMrequestIssued == false))
            {
            }
            // DRAMrequestIssued = true now
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }

            sub(currInst.cirs);
            //exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: sub (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[1]++;

            break;
        }
        case 2:
        { //mul
            if ((DRAMrequestIssued == false))
            {
            }
            // DRAMrequestIssued = true now
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }

            mul(currInst.cirs);
            //exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: Mul (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[2]++;

            break;
        }
        case 3:
        { // slt
            if ((DRAMrequestIssued == false))
            {
            }
            // DRAMrequestIssued = true now
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }

            slt(currInst.cirs);
            //exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]) );
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: slt (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));
            numOfInst[3]++;

            break;
        }

        case 4:
        { //addi
            if ((DRAMrequestIssued == false))
            {
            }
            // DRAMrequestIssued = true now
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }

            addi(currInst.cirs);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "Instruction: addi (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + to_string(currInst.cirs[2]) + ")" + ": " + rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]));

            numOfInst[4]++;

            break;
        }

        // NOTE THAT WHENEVER YOU ARE BRANCHING, YOU HAVE TO WAIT IF THE REGISTERS ARE NOT AVAILABLE BECAUSE YOU CANNOT DECIDE WHICH INSTRUCTION TO EXECUTE FIRST
        // HENCE WAIT UNTILL ALL CLOCK CYCLES TO LOAD THE NECESSARY REGISTER AND THEN EXECUTE THE CURRENT COMMAND.
        case 5:
        { //bne
            if ((DRAMrequestIssued == false))
            {
            }
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }

            bne(currInst.cirs, currInst.label);
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction bne (" + rrmap.at(currInst.cirs[0]) + "," + rrmap.at(currInst.cirs[1]) + "," + currInst.label + "," + to_string(switchOnBranch) + ")");
            numOfInst[5]++;

            break;
        }
        case 6:
        { //beq
            if ((DRAMrequestIssued == false))
            {
            }
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }
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
            if ((DRAMrequestIssued == false))
            {
                exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": " + "DRAM Request Issued (lw)");
                DRAMrequestIssued = true;
                lwInstRegisterID = currInst.cirs[0];

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
                //exectutionOutput.push_back("cycle " + to_string(numClockCycles + 1)  + "-" + to_string(uptoClkCyc) + ": " +  rrmap.at(currInst.cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]) );
                numOfInst[7]++;
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

            if ((DRAMrequestIssued == false))
            {
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
            break;
        }

        case 9:
        { //j       // note that here j instruction needs to wait before the DRAM calls are finished.

            if ((DRAMrequestIssued == false))
            {
            }
            else
            { // you cannot execute the instruction currently and cannot go forward => wait
                numClockCycles = uptoClkCyc + 1;
                DRAMrequestIssued = false; // that request has been completed now and we have that register's value in that register after loading from memory.
                lwInstRegisterID = -1;
                swInstMemAdd = -1;
                blockRegisterID = -1;
                blockMemoryAdd = -1;
            }
            j(currInst.cirs, currInst.label);
            numOfInst[9]++;
            exectutionOutput.push_back("cycle " + to_string(numClockCycles) + ": Instruction j :" + "Jump to label ID: " + currInst.label);
            break;
        }
        };
    }

    if (error1 != "")
    {
        cout << "Runtime Error:" << pc << ":" << error1 << endl;
        return;
    }
    return;
}