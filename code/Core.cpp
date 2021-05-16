#include "Core.hpp"   





CORE::CORE (string inFileName, int id, string outFileName,int nc,int DramCols,int DramRows, OutputHandler * ho, int rowDelay, int colDelay)
    {
        core_id = id;
        runtimeError = "";
        error = *(new vector<string>());
        CIRS = *(new vector<int>());
        sizeOfData = 0;
        numOfInst = *(new vector<int>());
        numberofInst = 0;
        numCores = nc;
        Dramcols = DramCols;
        Dramrows = DramRows;
        offsetOfCore = ((id-1)*((Dramcols)/numCores))*Dramrows;
        registers = new vector<int>(32,0);
        lableTable = *(new unordered_map <string,int>());
        uniqueLabelID = *(new unordered_map <int,string>());
        numberOfLine = 0;
        currentInstNum = 0;
        maxInstructions = 0;
        currentInst = "";
        labelNumber = 0;
        pc = 1;
        inputFileName = inFileName;   
        outputFileName = outFileName;
        inputprogram =*(new vector<string>());
        iset = *(new vector<instruction> ());
        //requestQueue = new vector<Request*>();
        minCost = 0;
        minCostRequest = nullptr;
        freeBuffer = new vector<Request*>();
        stalled = false;
        stallIfFull = false;
        stallingRequest = nullptr;
        switchOnBranch = false;
        writingFromDRAM = false;
        switchOnBranch = false;
        handleOutput = ho;
        rowAccessDelay = rowDelay;
        colAccessDelay = colDelay;

        rmap = *(new unordered_map<string,int> ());
        rrmap = *(new unordered_map<int,string> ());
        
        initialize_registers();

        for (int i = 0; i < 4; i++) {
            CIRS.push_back(0);
        }
        
        for (int i = 0; i < 10; i++) {
            numOfInst.push_back(0);
        }

        minCost = INT_MAX;

        working = true;
        hasSyntaxError = false; 
        hasRuntimeError = false;
      
        lexFile (inputFileName); // * Now our inputprogram is ready to parse
        preprocess();   // * Now our iset is ready
        
        if (error.size() > 0) {
            working = false;
            hasSyntaxError = true;
            handleOutput->updateNumOfInstForCore (core_id, &numOfInst);

            cout << endl << "Syntax Errors in Core " << core_id << endl;
		    for (int i = 0; i < error.size(); i++) cout << error[i] << endl;
	        cout << "Core " << core_id << " is not functioning" << endl;
        }

        stalled = false;
        stallIfFull = false;
        stallingRequest = nullptr;
        isFromFreeBuffer = false;
        
        postponedInstruction = *(new instruction ());

    }

void CORE::printIset () {
    cout << endl << "for core number " << core_id << endl;
    for (auto i = iset.begin(); i != iset.end(); i++) {
        cout << (*i).opID << " " << (*i).cirs[0] << " " << (*i).cirs[1] << " " <<(*i).cirs[2] << endl;
    }
}

void CORE::resume (bool sif) {
    stalled = false;
    if (sif) stallIfFull = false;
    cout << "inside resume" << endl;
    handleOutput->appendOutputForCore (core_id," coreId: " + to_string (core_id) + " RESUMED: ");
}

void CORE::addInFreeBuffer (Request * request) {
    freeBuffer->push_back(request);
}

void CORE::smoothExit () {
    //cout << "smoothExit called " << core_id<< endl;
    handleOutput->appendOutputForCore (core_id,": Exited :");
}

bool CORE::isRunnable () {
    return  working && ((pc <= iset.size()) || stalled) ;
} 
bool CORE::isWorking () {return working;}
bool CORE::isStalled () {return stalled;}

void CORE::setMinCostRequest (Request *r) {minCostRequest = r;}
Request* CORE::getMinCostRequest () {return minCostRequest;}

Request * CORE::getStallingRequest () { return stallingRequest; }
 
Request* CORE::getRequestWithMinCost () { return minCostRequest; }

void CORE::setRuntimeError (string s) {
    runtimeError = s;
    hasRuntimeError = true;
}


void CORE::stall (Request * request, bool sif) {
        stalled = true;
        stallIfFull = sif;
        cout << "hello " << request->inst->opID << endl;
        //cout <<" coreId: \t"<<core_id<<"STALLED";
        handleOutput->appendOutputForCore (core_id," coreId: " + to_string (core_id) + ": Stalling :");
        stallingRequest = request;
    }

void CORE::run (MRM *memoryRequestManager) {
    if (!working) return;
    if (hasRuntimeError) {
        working = false; 
        cout << "Runtime Error in Core " << core_id << runtimeError << endl; 
        cout << "Core " << core_id << " is not running" << endl;
        return;
    }

    if (stalled) {
        handleOutput->appendOutputForCore (core_id,": STALLED :");
        return;
    }

    instruction* currentInstruction = new instruction ();
       
    if (postponedInstruction.opID > -1) {
        currentInstruction->opID = postponedInstruction.opID;
        currentInstruction->cirs = postponedInstruction.cirs;
        currentInstruction->label = postponedInstruction.label;
        currentInstruction->cirsValue = postponedInstruction.cirsValue;
        postponedInstruction = *(new instruction ());
    }
    else {
        if (stallingRequest != nullptr) {
            currentInstruction = stallingRequest->inst;
            stallingRequest = nullptr;
        }
        else {
            if (freeBuffer->size() > 0) {
                cout << "jello " << endl;
                currentInstruction = freeBuffer->at(0)->inst;
                freeBuffer->erase (freeBuffer->begin());
                isFromFreeBuffer = true;
            }
            else {
                if (pc > iset.size()) {
                    smoothExit();
                    return;
                }
                else {
                    currentInstruction = &iset[pc - 1];
                    pc++;
                }
            }
        }
    }
    
    // * now we know which instruction to execute
    
    switch (currentInstruction->opID) {
        case 0: {
            if (currentInstruction->cirs[0] == 0) {
                hasRuntimeError = true; 
                runtimeError += " : Cannot modify $zero register in add instruction";
            }
            else {
                Request * request = new Request(0,core_id,currentInstruction,this);
                if (isFromFreeBuffer) {
                    isFromFreeBuffer = false;
                    if (writingFromDRAM) {
                        writingFromDRAM = false;
                        postponedInstruction = (*currentInstruction);
                        handleOutput->appendOutputForCore (core_id,": Instruction: add (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                    }
                    else {
                        add (currentInstruction->cirs);
                        handleOutput->appendOutputForCore (core_id,": Instruction: add (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                        numOfInst[0]++;
                    }
                    
                }
                else {
                    bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                    if (dependent) {
                        bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                        if (!enqueued) stall(request,true);
                        else handleOutput->appendOutputForCore (core_id,": Enqueuing Instruction: add (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));                        
                    }
                    else {
                        if (writingFromDRAM) {
                            writingFromDRAM = false;
                            postponedInstruction = (*currentInstruction);
                            handleOutput->appendOutputForCore (core_id,": Instruction: add (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                        }
                        else {
                            add (currentInstruction->cirs);
                            handleOutput->appendOutputForCore (core_id,": Instruction: add (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                            numOfInst[0]++;
                        }
                        
                    }
                }
            }
            break;
        }
        case 1: {
            if (currentInstruction->cirs[0] == 0) {
                hasRuntimeError = true; 
                runtimeError += " : Cannot modify $zero register in sub instruction";
            }
            else {
                if (isFromFreeBuffer) {
                    isFromFreeBuffer = false;
                    if (writingFromDRAM) {
                        writingFromDRAM = false;
                        postponedInstruction = (*currentInstruction);
                        handleOutput->appendOutputForCore (core_id,": Instruction: sub (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                    }
                    else {
                        sub (currentInstruction->cirs);
                        handleOutput->appendOutputForCore (core_id,": Instruction: sub (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));                
                        numOfInst[1]++;
                    }
                }
                else {
                    Request * request = new Request(0,core_id,currentInstruction,this);
                    bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                    if (dependent) {
                        bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                        if (!enqueued) stall(request,true);
                        else handleOutput->appendOutputForCore (core_id,":Enqueuing Instruction: sub (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));

                    }
                    else {
                        if (writingFromDRAM) {
                            writingFromDRAM = false;
                            postponedInstruction = (*currentInstruction);
                            handleOutput->appendOutputForCore (core_id,": Instruction: sub (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                        }
                        else {
                        sub (currentInstruction->cirs);
                        handleOutput->appendOutputForCore (core_id,": Instruction: sub (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));                
                        numOfInst[1]++;
                        }
                        
                    }
                }
            }
            break;
        }
        case 2: {
            if (currentInstruction->cirs[0] == 0) {
                hasRuntimeError = true; 
                runtimeError += " : Cannot modify $zero register in mul instruction";
            }
            else {
                if (isFromFreeBuffer) {
                    isFromFreeBuffer = false;
                    if (writingFromDRAM) {
                        writingFromDRAM = false;
                        postponedInstruction = (*currentInstruction);
                        handleOutput->appendOutputForCore (core_id,": Instruction: mul (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                    }
                    else {
                        mul (currentInstruction->cirs);
                        handleOutput->appendOutputForCore (core_id,": Instruction: mul (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                        numOfInst[2]++;
                    }   
                }
                else {
                    Request * request = new Request(0,core_id,currentInstruction,this);
                    bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                    if (dependent) {
                        bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                        if (!enqueued) stall(request,true);
                        else handleOutput->appendOutputForCore (core_id,":Enqueuing Instruction: mul (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));

                    }
                    else {
                        if (writingFromDRAM) {
                            writingFromDRAM = false;
                            postponedInstruction = (*currentInstruction);
                            handleOutput->appendOutputForCore (core_id,": Instruction: mul (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                        }
                        else {
                            mul (currentInstruction->cirs);
                            handleOutput->appendOutputForCore (core_id,": Instruction: mul (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                            numOfInst[2]++;
                        }
                    }
                }
            }
            break;
        }
        case 3: {
            if (currentInstruction->cirs[0] == 0) {
                hasRuntimeError = true; 
                runtimeError += " : Cannot modify $zero register in slt instruction";
            }
            else {
                 if (isFromFreeBuffer) {
                    isFromFreeBuffer = false;
                    if (writingFromDRAM) {
                        writingFromDRAM = false;
                        postponedInstruction = (*currentInstruction);
                        handleOutput->appendOutputForCore (core_id,": Instruction: slt (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                    }
                    else {
                        slt (currentInstruction->cirs);
                        handleOutput->appendOutputForCore (core_id,": Instruction: slt (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                        numOfInst[3]++;
                    }
                }
                else {
                    Request * request = new Request(0,core_id,currentInstruction,this);
                    bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                    if (dependent) {
                        bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                        if (!enqueued) stall(request,true);
                        else handleOutput->appendOutputForCore (core_id,":Enqueuing Instruction: slt (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));

                    }
                    else {
                        if (writingFromDRAM) {
                            writingFromDRAM = false;
                            postponedInstruction = (*currentInstruction);
                            handleOutput->appendOutputForCore (core_id,": Instruction: slt (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                        }
                        else {
                            slt (currentInstruction->cirs);
                            handleOutput->appendOutputForCore (core_id,": Instruction: slt (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                            numOfInst[3]++;
                        }
                    }    
                }
            }
            break;
        }
        case 4: {
            if (currentInstruction->cirs[0] == 0) {
                hasRuntimeError = true; 
                runtimeError += " : Cannot modify $zero register in addi instruction";
            }
            else {
                if (isFromFreeBuffer) {
                    isFromFreeBuffer = false;
                    if (writingFromDRAM) {
                        writingFromDRAM = false;
                        postponedInstruction = (*currentInstruction);
                        handleOutput->appendOutputForCore (core_id,": Instruction: addi (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + to_string(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                    }
                    else {
                        addi (currentInstruction->cirs);
                        handleOutput->appendOutputForCore (core_id,": Instruction: addi (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + to_string(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                        numOfInst[4]++;
                    }
                }
                else {
                    Request * request = new Request(0,core_id,currentInstruction,this);
                    bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                    if (dependent) {
                        bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                        if (!enqueued) stall(request,true);
                        else handleOutput->appendOutputForCore (core_id,":Enqueuing Instruction: addi (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + to_string(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));

                    }
                    else {
                        if (writingFromDRAM) {
                            writingFromDRAM = false;
                            postponedInstruction = (*currentInstruction);
                            handleOutput->appendOutputForCore (core_id,": Instruction: addi (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + to_string(currentInstruction->cirs[2]) + ")" + ": POSTPONED because write port is busy : ");
                        }
                        else {
                            addi (currentInstruction->cirs);
                            handleOutput->appendOutputForCore (core_id,": Instruction: addi (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + to_string(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers->at(currentInstruction->cirs[0])));
                            numOfInst[4]++;
                        }
                    }
                }
            }
            break;
        }
        case 5: {
            // ? bne beq do they come from free buffer?
            Request * request = new Request(0,core_id,currentInstruction,this);
            bool dependent = memoryRequestManager->checkDependencies(core_id, request);
            if (dependent) {
                cout << stalled << " stalled" << endl;
                bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                cout << "enqueued " << enqueued << endl;
                if (!enqueued) {
                    if (!stalled) stall(request,true);
                }
                else {
                    if (!stalled) stall(request,false);
                    handleOutput->appendOutputForCore (core_id,":Enqueuing Instruction bne (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + currentInstruction->label + "," + to_string(switchOnBranch) + ")");

                }
            }
            else {
                bne (currentInstruction->cirs, currentInstruction->label);
                handleOutput->appendOutputForCore (core_id,": Instruction bne (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + currentInstruction->label + "," + to_string(switchOnBranch) + ")");
                numOfInst[5]++;
            }
            break;
        }
        case 6: {
            Request * request = new Request(0,core_id,currentInstruction,this);
            bool dependent = memoryRequestManager->checkDependencies(core_id, request);
            if (dependent) {
                bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                if (!enqueued) {
                    if (!stalled) stall(request,true);
                }
                else {
                    if (!stalled) stall(request,false);
                    handleOutput->appendOutputForCore (core_id,":Enqueuing Instruction beq (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + currentInstruction->label + "," + to_string(switchOnBranch) + ")");

                }
            }
            else {
                beq (currentInstruction->cirs, currentInstruction->label);
                handleOutput->appendOutputForCore (core_id,": Instruction beq (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + currentInstruction->label + "," + to_string(switchOnBranch) + ")");
                numOfInst[6]++;

            }
            break;
        }
        case 7: {
            if (currentInstruction->cirs[0] == 0) {
                hasRuntimeError = true; 
                runtimeError += " : Cannot modify $zero register in lw instruction";
            }
            else {
                Request * request = new Request(0,core_id,currentInstruction,this);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);        
                bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                if (!enqueued) stall(request,true);
                else handleOutput->appendOutputForCore (core_id,"Enqueueing LW Request: " + rrmap.at(request->changingRegister) + "= @Address:" + to_string(request->loadingMemoryAddress) +"IN DRAM\t");
            }
            break;
        }
        case 8: {
            Request * request = new Request(0,core_id,currentInstruction,this);

            bool dependent = memoryRequestManager->checkDependencies(core_id, request);
            bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
            if (!enqueued) stall(request,true);
            else handleOutput->appendOutputForCore (core_id,"Enqueueing SW Request: " + rrmap.at(request->inst->cirs[0]) + "--> @Address:" + to_string(request->savingMemoryAddress) +"IN DRAM\t");
            
            break;
        } 
        case 9: {
            j (currentInstruction->cirs,currentInstruction->label);
            //cout << " coreId: "<<core_id <<" -> "<<": Instruction j :" << "Jump to label ID: " + currentInstruction->label+"\t";
            handleOutput->appendOutputForCore (core_id,": Instruction j : Jump to label ID: " + currentInstruction->label);
            numOfInst[9]++;
            break;
        }
        default: {}

    }
    //handleOutput->updateNumOfInstForCore (core_id, &numOfInst);
    // int updateMinCostAndRequest = findMinCost ();
    //cout <<core_id << " core:   last mein minCost " << minCost << endl;
    //if(minCostRequest!=nullptr) cout << core_id << " core:   mincost request's opid " << minCostRequest->inst->opID << endl;
}

vector<int> * CORE::getNumOfInst ()
{
    return &numOfInst;
}

void CORE::updateNumOfInst (int instOpId) {
    numOfInst[instOpId]++;
}

 void CORE::lexFile(string file)
    {
        ifstream ifs(file);
        string myline;
        while (ifs.good())
        {
            getline(ifs, myline);
            numberOfLine++;
            inputprogram.push_back(myline);
        }
        ifs.close();
        return;
    }   

 void CORE::initialize_registers()
{
    rmap["$zero"] = 0;
    rmap["$0"] = 0;
    rmap["$at"] = 1;
    rmap["$1"] = 1;
    rmap["$v0"] = 2;
    rmap["$2"] = 2;
    rmap["$v1"] = 3;
    rmap["$3"] = 3;
    rmap["$a0"] = 4;
    rmap["$4"] = 4;
    rmap["$a1"] = 5;
    rmap["$5"] = 5;
    rmap["$a2"] = 6;
    rmap["$6"] = 6;
    rmap["$a3"] = 7;
    rmap["$7"] = 7;
    rmap["$t0"] = 8;
    rmap["$8"] = 8;
    rmap["$t1"] = 9;
    rmap["$9"] = 9;
    rmap["$t2"] = 10;
    rmap["$10"] = 10;
    rmap["$t3"] = 11;
    rmap["$11"] = 11;
    rmap["$t4"] = 12;
    rmap["$12"] = 12;
    rmap["$t5"] = 13;
    rmap["$13"] = 13;
    rmap["$t6"] = 14;
    rmap["$14"] = 14;
    rmap["$t7"] = 15;
    rmap["$15"] = 15;
    rmap["$s0"] = 16;
    rmap["$16"] = 16;
    rmap["$s1"] = 17;
    rmap["$17"] = 17;
    rmap["$s2"] = 18;
    rmap["$18"] = 18;
    rmap["$s3"] = 19;
    rmap["$19"] = 19;
    rmap["$s4"] = 20;
    rmap["$20"] = 20;
    rmap["$s5"] = 21;
    rmap["$21"] = 21;
    rmap["$s6"] = 22;
    rmap["$22"] = 22;
    rmap["$s7"] = 23;
    rmap["$23"] = 23;
    rmap["$t8"] = 24;
    rmap["$24"] = 24;
    rmap["$t9"] = 25;
    rmap["$25"] = 25;
    rmap["$k0"] = 26;
    rmap["$26"] = 26;
    rmap["$k1"] = 27;
    rmap["$27"] = 27;
    rmap["$gp"] = 28;
    rmap["$28"] = 28;
    rmap["$sp"] = 29;
    rmap["$29"] = 29;
    rmap["$fp"] = 30;
    rmap["$30"] = 30;
    rmap["$ra"] = 31;
    rmap["$31"] = 31;

    rrmap[0] = "$zero";
    rrmap[1] = "$at";
    rrmap[2] = "$v0";
    rrmap[3] = "$v1";
    rrmap[4] = "$a0";
    rrmap[5] = "$a1";
    rrmap[6] = "$a2";
    rrmap[7] = "$a3";
    rrmap[8] = "$t0";
    rrmap[9] = "$t1";
    rrmap[10] = "$t2";
    rrmap[11] = "$t3";
    rrmap[12] = "$t4";
    rrmap[13] = "$t5";
    rrmap[14] = "$t6";
    rrmap[15] = "$t7";
    rrmap[16] = "$s0";
    rrmap[17] = "$s1";
    rrmap[18] = "$s2";
    rrmap[19] = "$s3";
    rrmap[20] = "$s4";
    rrmap[21] = "$s5";
    rrmap[22] = "$s6";
    rrmap[23] = "$s7";
    rrmap[24] = "$t8";
    rrmap[25] = "$t9";
    rrmap[26] = "$k0";
    rrmap[27] = "$k1";
    rrmap[28] = "$gp";
    rrmap[29] = "$sp";
    rrmap[30] = "$fp";
    rrmap[31] = "$ra";
    return;
}

void CORE::RemoveSpaces()
{
    int32_t j = 0;
    while (j < currentInst.size() && (currentInst[j] == ' ' || currentInst[j] == '\t'))
        j++;
    currentInst = currentInst.substr(j); //remove all of those
    return;
}

void CORE::findRegister(int i)
{

    if (currentInst[0] != '$' || currentInst.size() < 2)
    {
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Register Not Found");
        return; // raise exception
    }

    string first5 = "", first3 = "", first2 = "";

    first2 = currentInst.substr(0, 2);
    if (currentInst.size() >= 3)
        first3 = currentInst.substr(0, 3);
    if (currentInst.size() >= 5)
        first5 = currentInst.substr(0, 5);

    if (first5 == "$zero")
    {
        CIRS[i] = 0;
        currentInst = currentInst.substr(5);
        return;
    }
    if (rmap.find(first3) != rmap.end())
    {
        CIRS[i] = rmap.at(first3);
        currentInst = currentInst.substr(3);
        return;
    }
    if (rmap.find(first2) != rmap.end())
    {
        CIRS[i] = rmap.at(first2);
        currentInst = currentInst.substr(2);
        return;
    }

    error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unknown Register");
    return;
}

void CORE::removeComma()
{
    if (currentInst.size() < 2 || currentInst[0] != ',')
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Comma Expected");
    currentInst = currentInst.substr(1);
    return;
}

void CORE::findImmediate()
{
    int i = 0;
    bool isValidInt = true;
    string buffer;
    if (currentInst[0] == '-')
    {
        buffer = "-";
        currentInst = currentInst.substr(1);
    }
    else
        i = 0;

    while (i < currentInst.size() && currentInst[i] < 58 && currentInst[i] > 47)
    {
        if (currentInst[i] == ' ' || currentInst[i] == '\t' || currentInst[i] == '(')
            break;
        buffer += currentInst[i];
        i++;
    }
    if (buffer == "") error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Integer constant value not found");
    else CIRS[2] = stoi(buffer);
    currentInst = currentInst.substr(i);
}

string CORE::findLabel()
{
    RemoveSpaces();
    string buffer = "";
    int i = 0;
    while (i < currentInst.size())
    {
        if (currentInst[i] == ' ' || currentInst[i] == '\t')
            break;
        buffer += currentInst[i];
        i++;
    }

    currentInst = currentInst.substr(i);
    RemoveSpaces();
    if (!currentInst.empty())
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Invalid Label Type");
    return buffer;
}

void CORE::offsetType()
{
    string buffer = "";
    string buffer1 = "";
    int i = 0;
    int offset = 0;
    int final1 = 0;
    bool bracketed = true;
    if (currentInst[0] == '-')
    {
        buffer = "-";
        currentInst = currentInst.substr(1);
    }
    else
        i = 0;

    while (i < currentInst.size() && currentInst[i] < 58 && currentInst[i] > 47)
    {
        if (currentInst[i] == ' ' || currentInst[i] == '\t' || currentInst[i] == '(')
            break;
        buffer += currentInst[i];
        i++;
    }
    currentInst = currentInst.substr(i);
    RemoveSpaces();
    int indexOfLparen = currentInst.find("(");
    if (indexOfLparen == -1)
        bracketed = false;
    else
        bracketed = true;

    int indexOfRparen = currentInst.find(")");
    if (bracketed && (indexOfRparen == -1 || indexOfRparen < indexOfLparen))
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unmatched Parenthesis");
    else if (bracketed)
    {

        string strInsideParen;
        if (bracketed)
            strInsideParen = currentInst.substr(indexOfLparen + 1, (indexOfRparen - indexOfLparen - 1));
        else
            strInsideParen = currentInst;

        RemoveSpaces();
        if (strInsideParen.size() > 0)
        {
            RemoveSpaces();
            int ValOfRegister = 0;
            int offset1 = 0;

            if (strInsideParen[0] == '$')
            {
                if (strInsideParen.size() < 2)
                {
                    error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Register Not Found");
                    return; // raise exception
                }

                string first5 = "", first3 = "", first2 = "";

                first2 = strInsideParen.substr(0, 2);
                if (strInsideParen.size() >= 3)
                    first3 = strInsideParen.substr(0, 3);
                if (strInsideParen.size() >= 5)
                    first5 = strInsideParen.substr(0, 5);

                if (first5 == "$zero")
                {
                    buffer1 = first5;
                    ValOfRegister = registers->at(0);
                    strInsideParen = strInsideParen.substr(5);
                }
                else if (rmap.find(first3) != rmap.end())
                {
                    buffer1 = first3;
                    ValOfRegister = registers->at(rmap.at(first3));
                    strInsideParen = strInsideParen.substr(3);
                }
                else if (rmap.find(first2) != rmap.end())
                {
                    buffer1 = first2;
                    ValOfRegister = registers->at(rmap.at(first2));
                    strInsideParen = strInsideParen.substr(2);
                }
                else
                {
                    error.push_back("Synatx Error:" + to_string(currentInstNum) + ": Register Not Found");
                    return;
                };
                final1 = ValOfRegister;
            }

            else
            { // string inside parenthesis does not start with $
                int i1 = 0;

                if (strInsideParen[0] == '-')
                {
                    buffer1 = "-";
                    strInsideParen = strInsideParen.substr(1);
                }
                else
                    i1 = 0;

                while (i1 < strInsideParen.size() && strInsideParen[i1] < 58 && strInsideParen[i1] > 47)
                {
                    if (strInsideParen[i1] == ' ' || strInsideParen[i1] == '\t' || strInsideParen[i1] == ')')
                        break;
                    buffer1 += strInsideParen[i1];
                    i1++;
                }
            }
        }

        if (rmap.find(buffer1) != rmap.end())
        {
            CIRS[3] = 0;
            CIRS[2] = rmap.at(buffer1);
        }
        else
        {
            CIRS[3] = 1;
            for (int i = 0; i < buffer1.size(); i++)
            {
                if ((buffer1[i] <= 47 || buffer1[i] >= 58) && buffer1[i] != '-')
                {
                    error.push_back("Synatx Error:" + to_string(currentInstNum) + ": Not a valid parameter Inside Parenthesis");
                    return;
                }
            }
            CIRS[2] = stoi(buffer1);
        }

        if (bracketed)
            currentInst = currentInst.substr(indexOfRparen + 1);
        else
            currentInst = currentInst.substr(0);
    }

    for (int i = 0; i < buffer.size(); i++)
    {
        if ((buffer[i] <= 47 || buffer[i] >= 58) && buffer[i] != '-')
        {
            error.push_back("Synatx Error:" + to_string(currentInstNum) + ": Not a Valid Offset");
            return;
        }
    }
    offset = stoi(buffer);
    offset += offsetOfCore;

    CIRS[1] = offset;
    return;
}

instruction CORE::readInst()
{
    int j = 0;
    string lableInst;
    vector<int> mynullv = {-1, -1, -1};
    instruction mynull(mynullv, -1);

    if (currentInst == "")
        return mynull;
    if (currentInst.size() < 4)
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Instruction Not Valid");
    for (j = 0; j < 4; j++)
        if (currentInst[j] == ' ' || currentInst[j] == '\t')
            break;

    string operation = currentInst.substr(0, j);
    currentInst = currentInst.substr(j + 1);

    bool isValidOp = false;
    int operationId = -1;
    for (int i = 0; i < myset.size(); i++)
    {
        if (myset[i] == operation)
        {
            isValidOp = true;
            operationId = i;
            break;
        }
    }

    if ((0 <= operationId) && (operationId < 4))
    { //add,sub,mul,slt
        for (int i = 0; i < 3; i++)
        {
            RemoveSpaces();
            
            findRegister(i);
            RemoveSpaces();
            if (i == 2)
                break;
            removeComma();
        }
        RemoveSpaces();
        if (currentInst != "")
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
    }

    else if (operationId == 4)
    { //addi
        for (int i = 0; i < 2; i++)
        {
            RemoveSpaces();
            findRegister(i);
            RemoveSpaces();
            if (i == 1)
                break;
            removeComma();
        }

        RemoveSpaces();
        removeComma();
        RemoveSpaces();
        findImmediate();
        RemoveSpaces();
        if (currentInst != "")
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
    }

    else if (operationId == 5 || operationId == 6)
    { //bne,beq
        for (int i = 0; i < 2; i++)
        {
            RemoveSpaces();
            findRegister(i);
            RemoveSpaces();
            if (i == 1)
                break;
            removeComma();
        }

        RemoveSpaces();
        removeComma();
        lableInst = findLabel();
        RemoveSpaces();
        if (currentInst != "")
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
    }

    else if (operationId == 7 || operationId == 8)
    { //lw,sw
        string tempstring = "";
        int offset;
        RemoveSpaces();
        findRegister(0);
        RemoveSpaces();
        removeComma();
        RemoveSpaces();
        if ((currentInst[0] > 47 && currentInst[0] < 58) || currentInst[0] == '-')
            offsetType();
        else
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unacceptable Label Type");
    }

    else if (operationId == 9)
    { //j label/address
        RemoveSpaces();
        lableInst = findLabel();
    }
    else if (operationId == -1)
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unknown Instruction");

    if (operationId == 9 || operationId == 5 || operationId == 6)
    {
        instruction inst(CIRS, operationId, lableInst);
        return inst;
    }
    else
    {
        instruction inst(CIRS, operationId);
        //cout<<operationId<<endl;
        return inst;
    }
}

void CORE::preprocess()
{ // checking if line has label: at the beginning;
    int labelIndex;
    currentInstNum = 1;
    string tempLabel = "";
    for (int i = 0; i < numberOfLine; i++)
    {
        CIRS = {0, 0, 0, 0}; // re initializing CIRS to remove previous instructions data if any
        bool isLabel = false;
        currentInst = inputprogram[i];
        RemoveSpaces();
        if (currentInst != "")
        {
            RemoveSpaces();

            if (count(currentInst.begin(), currentInst.end(), ':') > 1)
                error.push_back("Syntax Error:" + to_string(currentInstNum) + ": \": Error: more than one label");
            labelIndex = currentInst.find(":");

            if (labelIndex == -1)
                isLabel = false;
            else if (labelIndex == 0)
                error.push_back("Syntax Error:" + to_string(currentInstNum) + ": \":\" At The Beginning Not Accepted");
            else
            {
                tempLabel = currentInst.substr(0, labelIndex);
                lableTable[tempLabel] = currentInstNum;
                currentInst = currentInst.substr(labelIndex);
                RemoveSpaces();
                currentInst = currentInst.substr(1);
                RemoveSpaces();
                if (currentInst == "")
                    continue;
            }
        }
        else
            continue;
        instruction instObj = readInst();
        iset.push_back(instObj);
        numberofInst++;
        currentInstNum++;
    }
}

////////////////////////////////////////////////////

string CORE::getRuntimeError () {return runtimeError;}
int CORE::getMinCost () {return minCost;}
void CORE::setMinCost (int c) {minCost = c;}
vector<int> * CORE::getRegisters () {return registers;}

void CORE::add(vector<int> &cirs) { registers->at(cirs[0]) = registers->at(cirs[1]) + registers->at(cirs[2]); }
void CORE::sub(vector<int> &cirs) { registers->at(cirs[0]) = registers->at(cirs[1]) - registers->at(cirs[2]); }
void CORE::mul(vector<int> &cirs) { registers->at(cirs[0]) = registers->at(cirs[1]) + registers->at(cirs[2]); }
void CORE::addi(vector<int> &cirs) { registers->at(cirs[0]) = registers->at(cirs[1]) + cirs[2]; }
void CORE::bne(vector<int> &cirs, string &label)
{
    if (lableTable.find(label) == lableTable.end())
    {
        runtimeError = "Error: Label Not Found";
        return;
    }
    if (registers[cirs[0]] != registers[cirs[1]])
    {
        if (lableTable.find(label) != lableTable.end())
        {
            pc = lableTable[label];
            switchOnBranch = true;
        }
        else
        {
            runtimeError = "Error: Label Not Found";
            return;
        }
    }
    else
        switchOnBranch = false;
    return;
}
void CORE::beq(vector<int> &cirs, string &label)
{
    if (lableTable.find(label) == lableTable.end())
    {
        runtimeError = "Error: Label Not Found";
        return;
    }

    if (registers[cirs[0]] == registers[cirs[1]])
    {
        if (lableTable.find(label) != lableTable.end())
        {
            pc = lableTable[label];
            switchOnBranch = true;
        }
        else
        {
            runtimeError = "Error: Label Not Found";
            return;
        }
    }
    else
        switchOnBranch = false;
    return;
}
void CORE::slt(vector<int> &cirs)
{
    if (registers[cirs[1]] < registers[cirs[2]])
        registers->at(cirs[0]) = 1;
    else
        registers->at(cirs[0]) = 0;
    return;
}
void CORE::j(vector<int> &cirs, string &label) { pc = lableTable[label]; }