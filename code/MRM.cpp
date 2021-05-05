#include "MRM.hpp"


MRM::MRM (DRAM * d,vector<CORE*> * ac, int nc, int rad, int cad,OutputHandler * ho, int cql = 20) {
    dram = d;
    numCores = nc;
    DRAMrequestIssued = false;
    Delay=1;
    coreQueueLength = cql;

    rowAccessDelay = rad;
    colAccessDelay = cad;

    allCoreQueues = new vector<vector<Request*>*> (numCores);
    allCores = ac;
    for (int i=0; i<numCores; i++) {
        allCoreQueues->at (i) = new vector<Request*> ();
    }
    currentRequestInDRAM = nullptr;
    uptoClkCycle = -1;
    uptoDelayClkCycle = -1;
    handleOutput = ho;
    decCost = 0.01;
}

vector<Request*> * MRM::getQueueOfCore_id (int id) {
    return allCoreQueues->at(id - 1);
}

bool MRM::isDependent (Request* srcRequest, Request * dstRequest) {
        switch (dstRequest->inst->opID) {
                case 0: // add
                case 1: // sub
                case 2: //mul
                case 3: { // slt 
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister || dstRequest->inst->cirs[1] == srcRequest->changingRegister || dstRequest->inst->cirs[2] == srcRequest->changingRegister) return true;
                    break;
                }
                case 4: { // addi 
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister || dstRequest->inst->cirs[1] == srcRequest->changingRegister) return true;
                    break;
                }
                case 5: // bne
                case 6: { //beq
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister || dstRequest->inst->cirs[1] == srcRequest->changingRegister) return true;
                    break;
                }
                case 7:  { //lw
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister || dstRequest->inst->cirs[2] == srcRequest->changingRegister || dstRequest->loadingMemoryAddress == srcRequest->savingMemoryAddress) return true;
                    break;
                }
                case 8: { // sw
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister || dstRequest->inst->cirs[2] == srcRequest->changingRegister || dstRequest->savingMemoryAddress == srcRequest->loadingMemoryAddress || dstRequest->savingMemoryAddress == srcRequest->savingMemoryAddress) return true;
                    // ! sw will have problem with other lw and sw
                    break;
                } 
                case 9: { // j
                    return false; // ! HANDLE ERROR (I think this is enough now?)
                    break;
                }
                default: 
                    break;
        }
        return false;
    }

bool MRM::checkDependencies ( int id,Request * r) {
        vector<Request*> * coreQueue  = getQueueOfCore_id (id);
        bool dependent = false;
        for (auto requestInQueue = coreQueue->begin(); requestInQueue != coreQueue->end(); requestInQueue++) {
            switch (r->inst->opID) {
                case 0: // add
                case 1: // sub
                case 2: //mul
                case 3: { // slt 
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister || r->inst->cirs[1] == (*requestInQueue)->changingRegister || r->inst->cirs[2] == (*requestInQueue)->changingRegister ) {
                        r->cost += ceil ((*requestInQueue)->cost) + 1;
                        r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + 1));
                        (*requestInQueue)->cost -= decCost;
                        dependent = true;
                    }
                    break;
                }
                case 4: { // addi 
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister || r->inst->cirs[1] == (*requestInQueue)->changingRegister) {
                        r->cost += ceil ((*requestInQueue)->cost) + 1;
                        r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost)+1));
                        (*requestInQueue)->cost -= decCost;
                        dependent = true;
                    }
                    break;
                }
                
                case 5: // bne
                case 6: { //beq
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister || r->inst->cirs[1] == (*requestInQueue)->changingRegister) {
                        r->cost += ceil ((*requestInQueue)->cost) + 1;
                        r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + 1));
                        (*requestInQueue)->cost -= decCost;
                        dependent = true;
                    }
                    break;
                }
                case 7:  { //lw
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister || r->inst->cirs[2] == (*requestInQueue)->changingRegister || r->loadingMemoryAddress == (*requestInQueue)->savingMemoryAddress) {
                        r->cost += ceil ((*requestInQueue)->cost);

                        (*requestInQueue)->cost -= decCost;
                        dependent = true;

                        if ((*requestInQueue)->inst->opID == 7) {
                            if (DRAM::getRowOfRowBuffer((*requestInQueue)->loadingMemoryAddress) == DRAM::getRowOfRowBuffer(r->loadingMemoryAddress)) {
                                if (DRAM::getColOfRowBuffer((*requestInQueue)->loadingMemoryAddress) == DRAM::getColOfRowBuffer(r->loadingMemoryAddress)) {
                                    r->cost += 2;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + 2));
                                }
                                else {
                                    r->cost += colAccessDelay + 1;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + colAccessDelay + 1));
                                }
                            }
                            else {
                                r->cost += (2 * rowAccessDelay) + colAccessDelay + 1;
                                r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + (2 * rowAccessDelay) + colAccessDelay + 1));
                            }
                        } 
                        else if ((*requestInQueue)->inst->opID == 8) {
                            if (DRAM::getRowOfRowBuffer((*requestInQueue)->savingMemoryAddress) == DRAM::getRowOfRowBuffer(r->loadingMemoryAddress)) {
                                if (DRAM::getColOfRowBuffer((*requestInQueue)->savingMemoryAddress) == DRAM::getColOfRowBuffer(r->loadingMemoryAddress)) {
                                    r->cost += 1 + 1;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + 1 + 1));
                                }
                                else {
                                    r->cost += colAccessDelay + 1;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + colAccessDelay + 1));
                                }
                            }
                            else {
                                r->cost += (2 * rowAccessDelay) + colAccessDelay + 1;
                                r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + (2 * rowAccessDelay) + colAccessDelay + 1));

                            }
                        } 
                    }
                    //if(!dependent) r->cost = (2*rowAccessDelay) + colAccessDelay;
                    break;
                }
                case 8: { // sw
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister || r->inst->cirs[2] == (*requestInQueue)->changingRegister || r->savingMemoryAddress == (*requestInQueue)->loadingMemoryAddress || r->savingMemoryAddress == (*requestInQueue)->savingMemoryAddress) {
                        r->cost += ceil ((*requestInQueue)->cost);
                        (*requestInQueue)->cost -= decCost;
                        dependent = true;

                        if ((*requestInQueue)->inst->opID == 7) {
                            if (DRAM::getRowOfRowBuffer((*requestInQueue)->loadingMemoryAddress) == DRAM::getRowOfRowBuffer(r->savingMemoryAddress)) {
                                if (DRAM::getColOfRowBuffer((*requestInQueue)->loadingMemoryAddress) == DRAM::getColOfRowBuffer(r->savingMemoryAddress)) {
                                    r->cost += 1 + 1;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + 1 + 1));
                                }
                                else {
                                    r->cost += colAccessDelay + 1;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + colAccessDelay + 1));
                                }
                            }
                            else {
                                r->cost += (2 * rowAccessDelay) + colAccessDelay + 1;
                                r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + (2 * rowAccessDelay) + colAccessDelay + 1));
                            }
                        } 
                        else if ((*requestInQueue)->inst->opID == 8) {
                            if (DRAM::getRowOfRowBuffer((*requestInQueue)->savingMemoryAddress) == DRAM::getRowOfRowBuffer(r->savingMemoryAddress)) {
                                if (DRAM::getColOfRowBuffer((*requestInQueue)->savingMemoryAddress) == DRAM::getColOfRowBuffer(r->savingMemoryAddress)) {
                                    r->cost += 1 + 1;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + 1 + 1));
                                }
                                else {
                                    r->cost += colAccessDelay + 1;
                                    r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + colAccessDelay + 1));
                                }
                            }
                            else {
                                r->cost += (2 * rowAccessDelay) + colAccessDelay + 1;
                                r->subtractCostQueue->push_back (make_pair((*requestInQueue),ceil ((*requestInQueue)->cost) + (2 * rowAccessDelay) + colAccessDelay + 1));
                            }
                        } 
                    }
                    //if(!dependent) r->cost = (2*rowAccessDelay) + colAccessDelay;
                    // ! sw will have problem with other lw and sw
                    break;
                } 
                case 9: { // j
                    // ! HANDLE ERROR
                    allCores->at(r->core_id - 1)->setRuntimeError("j instruction should not be sent for queueing");
                    break;
                }
                default: 
                    break;
            }
        }
        if ((r->inst->opID == 7 || r->inst->opID == 8) && (!dependent)) {
            r->cost = (2*rowAccessDelay) + colAccessDelay + 1;
            cout << "this is cost agag " << r->cost << endl;
        }
        //if (r->core_id == 1) cout << r->cost << " this is cost " << endl;
        return dependent;
    }

bool MRM::enqueueRequest (int coreId, Request * r) {
        vector<Request*> * coreQueue = getQueueOfCore_id(coreId);
        if (coreQueue->size() >= coreQueueLength) return false;
        else {
            coreQueue->push_back(r);

            if (r->cost < allCores->at(coreId - 1)->getMinCost()) {
                allCores->at(coreId - 1)->setMinCost (r->cost);
                allCores->at(coreId - 1)->setMinCostRequest (r);
            }
        }
        return true;
    }

CORE * MRM::selectCore (vector<CORE*> * allCores) {
        // * returns nullptr if there are no requests 
        int minCost = INT_MAX;
        CORE * coreWithMinCost = nullptr;
        for (auto core = allCores->begin(); core != allCores->end(); core++) {
            if ((*core)->getMinCost() < minCost) {
                minCost = (*core)->getMinCost();
                coreWithMinCost = (*core);
            }
        }
        return coreWithMinCost;
    }

Request* MRM::getMinCostRequest (vector<CORE*> * allCores,int& Delay) { 
        CORE* minCostCore = selectCore(allCores);
        if (minCostCore == nullptr) return nullptr; 
        Request* request = minCostCore->getRequestWithMinCost();
        return request;
    }


pair<Request *, int> MRM::getMinCostRequest (vector<Request*> * coreQueue) {
    int minCost = INT_MAX;
    Request * myMinCostRequest = nullptr;
    for (auto i = coreQueue->begin(); i != coreQueue->end(); i++) {
        if ((*i)->cost < minCost) {
            minCost = (*i)->cost;
            myMinCostRequest = (*i);
        }
    }
    return make_pair (myMinCostRequest, minCost);
}

int MRM::getCostToBeSubtracted (Request* r, vector<pair<Request*,int>> * requestQueue) {
    for (auto x = requestQueue->begin(); x != requestQueue->end(); x++) {
        if ((*x).first == r) {
            int answer = (*x).second;
            requestQueue->erase (x);
            return answer;
        }
    }
    return 0;
}
bool MRM::dequeueRequest (Request * r, vector<Request*> * coreQueue) {
        bool deleted = false;
        for (auto i = coreQueue->begin(); i != coreQueue->end(); i++) {
            if ( (*i) == r) {
                for (auto j = i+1; j != coreQueue->end(); j++) {
                    if (j != coreQueue->end() && isDependent(*i, *j)) {
                        (*j)->cost -= getCostToBeSubtracted ((*i), (*j)->subtractCostQueue) ;
                    }
                    
                }
                coreQueue->erase (i);
                pair<Request *, int> temp = getMinCostRequest (coreQueue);
                allCores->at(r->core_id - 1)->setMinCost (temp.second);
                allCores->at(r->core_id - 1)->setMinCostRequest (temp.first);
                deleted = true;
                break;
            }
        }
        
        return deleted;
    }

void MRM::execute (vector<CORE*> * allCores, int currentClockCycle) {
    /*
        for (auto itr = allCores->begin(); itr != allCores->end();itr++) {
            if (getQueueOfCore_id((*itr)->getCoreId())->size() < coreQueueLength && 
                (*itr)->isStalled()) (*itr)->resume(true);
        }
*/
        if (currentClockCycle > uptoClkCycle) {
            DRAMrequestIssued = false;
            handleOutput->addDramOutput (": Idle");
            currentRequestInDRAM = nullptr;
        }
        if (currentClockCycle == uptoClkCycle) {
            uptoClkCycle = -1;
            
            if (currentRequestInDRAM->inst->opID == 7) {
                dram->setRowBuffer(DRAM::getRowOfRowBuffer (currentRequestInDRAM->loadingMemoryAddress));
                dram->setColBuffer(DRAM::getColOfRowBuffer (currentRequestInDRAM->loadingMemoryAddress));

                cout <<" coreId: "<<currentRequestInDRAM->core_id <<  " ->  " + allCores->at(0)->rrmap.at(currentRequestInDRAM->changingRegister) + "=" + to_string(dram->getLoadedValueForLW ())+"\n";
                handleOutput->appendOutputForCore (currentRequestInDRAM->core_id, allCores->at(0)->rrmap.at(currentRequestInDRAM->changingRegister) + "=" + to_string(dram->getLoadedValueForLW ())+"\t");
                allCores->at (currentRequestInDRAM->core_id - 1)->getRegisters()->at (currentRequestInDRAM->changingRegister) = dram->getLoadedValueForLW ();
                allCores->at (currentRequestInDRAM->core_id - 1)->updateNumOfInst (7);

                handleOutput->addDramOutput (": LW for core " + to_string (currentRequestInDRAM->core_id) + " Done");
                dequeueRequest (currentRequestInDRAM, getQueueOfCore_id (currentRequestInDRAM->core_id));
            }
            else if (currentRequestInDRAM->inst->opID == 8) {
                dram->setRowBuffer(DRAM::getRowOfRowBuffer (currentRequestInDRAM->savingMemoryAddress));
                dram->setColBuffer(DRAM::getColOfRowBuffer (currentRequestInDRAM->savingMemoryAddress));

                cout <<" coreId: "<<currentRequestInDRAM->core_id << " -> memory address " + to_string(currentRequestInDRAM->savingMemoryAddress) + "-" + to_string(currentRequestInDRAM->savingMemoryAddress + 3) + " = " + to_string(dram->getValueToBeStoredForSW()) + "\n";
                handleOutput->appendOutputForCore (currentRequestInDRAM->core_id,": memory address " + to_string(currentRequestInDRAM->savingMemoryAddress) + "-" + to_string(currentRequestInDRAM->savingMemoryAddress + 3) + " = " + to_string(dram->getValueToBeStoredForSW()) + "\t");                
                
                int setted = dram->setMemory (currentRequestInDRAM->savingMemoryAddress,dram->getValueToBeStoredForSW());
                if (setted == -1) allCores->at(currentRequestInDRAM->core_id - 1)->setRuntimeError("Error: Memory Address not 123 accessible");
                else if (setted == -2) allCores->at(currentRequestInDRAM->core_id - 1)->setRuntimeError("Error: invalid memory 123 location");
                
                allCores->at (currentRequestInDRAM->core_id - 1)->updateNumOfInst (8);
                handleOutput->addDramOutput (": SW for core " + to_string(currentRequestInDRAM->core_id) + " Done");
                dequeueRequest (currentRequestInDRAM, getQueueOfCore_id (currentRequestInDRAM->core_id));
            }
            else allCores->at(currentRequestInDRAM->core_id - 1)->setRuntimeError("Request in DRAM cannot be other than sw and lw");
        }

        //if (currentClockCycle < uptoDelayClkCycle) {return;}
        Request* request = getMinCostRequest (allCores,Delay);
       

        if (request == nullptr) return;
       cout << "opid of mincost request" << request->inst->opID <<"    " << allCores->at(0)->rrmap.at(request->inst->cirs[0])  << "    " <<request->cost << endl;

        if (request->inst->opID != 7 && request->inst->opID != 8 && request->cost <= 1) {
            if  (allCores->at(request->core_id - 1)->getStallingRequest() == request && 
                getQueueOfCore_id(request->core_id)->size() == coreQueueLength) allCores->at(request->core_id)->resume(true);
            else if(allCores->at(request->core_id - 1)->getStallingRequest() == request) {
                allCores->at((request->core_id) - 1)->resume(false);
            }
            else {
                allCores->at(request->core_id - 1)->addInFreeBuffer(request);
            }

            dequeueRequest (request,getQueueOfCore_id(request->core_id));
            return;
        }

        if (request->inst->opID != 7 && request->inst->opID != 8 && request->cost > 1) {
            allCores->at(request->core_id - 1)->setRuntimeError("This should not happen");
            return;
        }
        
        if (DRAMrequestIssued) {
            // * calculate delay 
            if (currentRequestInDRAM->inst->opID == 7) handleOutput->addDramOutput (": Executing LW request for core " + to_string(currentRequestInDRAM->core_id) + ": @Address " + to_string (currentRequestInDRAM->loadingMemoryAddress) + " --> @Register " + allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[0]));
            else if (currentRequestInDRAM->inst->opID == 8) handleOutput->addDramOutput (": Executing SW request for core " + to_string(currentRequestInDRAM->core_id) + ": @Address " + to_string (currentRequestInDRAM->savingMemoryAddress) + " <-- @Value " + to_string (currentRequestInDRAM->storeThisForSW));
            else allCores->at(currentRequestInDRAM->core_id - 1)->setRuntimeError("not a lw or sw instruction sent in DRAM");
            uptoDelayClkCycle = currentClockCycle + 1;
        }
        else {

            DRAMrequestIssued = true;
            currentRequestInDRAM = request;

            if (request->inst->opID == 7) {
                //cout<<" coreId: "<<currentRequestInDRAM->core_id<<"\t" << ": DRAM request issued : lw\t";
                handleOutput->addDramOutput (": LW for core " + to_string(currentRequestInDRAM->core_id) + " Issued::[" +allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[0]) + "," +  to_string(currentRequestInDRAM->inst->cirs[1]) + "," + allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[2]) + "]");
                handleOutput->appendOutputForCore (currentRequestInDRAM->core_id,": LW for core " + to_string(currentRequestInDRAM->core_id) + " Issued in DRAM::[" +allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[0]) + "," +  to_string(currentRequestInDRAM->inst->cirs[1]) + "," + allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[2]) + "]");
                
                if (dram->getRowBuffer() == DRAM::getRowOfRowBuffer (request->loadingMemoryAddress)) {
                    if (dram->getColBuffer() == DRAM::getColOfRowBuffer (request->loadingMemoryAddress)) uptoClkCycle = currentClockCycle + 1;
                    else uptoClkCycle = currentClockCycle + colAccessDelay; 
                }
                else uptoClkCycle = currentClockCycle + (2 * rowAccessDelay) + colAccessDelay;

                currentRequestInDRAM = request;
                int success = dram->lw (request);
                if (success == -1) allCores->at(request->core_id - 1)->setRuntimeError("Error: Memory Address not accessible");
                else if (success == -2) allCores->at(request->core_id - 1)->setRuntimeError("Error: invalid memory location");
            }
            else if (request->inst->opID == 8) {
                //cout<<" coreId: "<<currentRequestInDRAM->core_id<<"\t" << ": DRAM request issued : sw\t";
                handleOutput->addDramOutput (": SW for core " + to_string(currentRequestInDRAM->core_id) + " Issued::[" +allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[0]) + "," +  to_string(currentRequestInDRAM->inst->cirs[1]) + "," + allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[2]) + "]");
                handleOutput->appendOutputForCore (currentRequestInDRAM->core_id,": SW for core " + to_string(currentRequestInDRAM->core_id) + " Issued in DRAM::[" +allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[0]) + "," +  to_string(currentRequestInDRAM->inst->cirs[1]) + "," + allCores->at(0)->rrmap.at(currentRequestInDRAM->inst->cirs[2]) + "]");
            
                if (dram->getRowBuffer() == DRAM::getRowOfRowBuffer (request->savingMemoryAddress)) {
                    if (dram->getColBuffer() == DRAM::getColOfRowBuffer (request->savingMemoryAddress)) uptoClkCycle = currentClockCycle + 1;
                    else uptoClkCycle = currentClockCycle + colAccessDelay; 
                }
                else uptoClkCycle = currentClockCycle + (2 * rowAccessDelay) + colAccessDelay;

                currentRequestInDRAM = request;
                int success = dram->sw (request);
                if (success == -1) allCores->at(request->core_id - 1)->setRuntimeError("Error: Memory Address not accessible");
                else if (success == -2) allCores->at(request->core_id - 1)->setRuntimeError("Error: invalid memory location");
            }
            else allCores->at(request->core_id - 1)->setRuntimeError("not a lw or sw instruction");
        }
        
        return;
    }