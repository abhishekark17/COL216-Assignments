#pragma once
#include "main.hpp"
#include "Core.hpp"
#include "DRAM.hpp"
#include "Request.hpp"
class MRM {
private:
    DRAM * dram;
    bool DRAMrequestIssued;
    int uptoClkCycle;
    int uptoDelayClkCycle;
    int Delay; 
    int numCores;
    int coreQueueLength;
    vector<vector<Request*> * >* allCoreQueues;
    vector<CORE*> * allCores;

    int rowAccessDelay , colAccessDelay;

    Request * currentRequestInDRAM;
public:
    MRM (DRAM * d,vector<CORE*> * ac, int nc, int rad, int cad, int cql = 20) {
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
     }

    vector<Request*> * getQueueOfCore_id (int id) {
        return allCoreQueues->at(id - 1);
    }


    // * This function updates the cost of input request depending upon what all instructions it is dependent on and returns if it is dependent or not
    // * let us keep the default cost of instruction as zero

    // * NOTE: dstRequest will be ahead of srcRequest
    bool isDependent (Request* srcRequest, Request * dstRequest) {
        switch (dstRequest->inst->opID) {
                case 0: // add
                case 1: // sub
                case 2: //mul
                case 3: { // slt 
                    if ( dstRequest->inst->cirs[1] == srcRequest->changingRegister) return true;
                    if ( dstRequest->inst->cirs[2] == srcRequest->changingRegister) return true;
                    break;
                }
                case 4: { // addi 
                    if ( dstRequest->inst->cirs[1] == srcRequest->changingRegister) return true;
                    break;
                }
                case 5: // bne
                case 6: { //beq
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister) return true;
                    if ( dstRequest->inst->cirs[1] == srcRequest->changingRegister) return true;
                    break;
                }
                case 7:  { //lw
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister) return true;
                    if ( dstRequest->inst->cirs[2] == srcRequest->changingRegister) return true;
                    if (dstRequest->loadingMemoryAddress == srcRequest->savingMemoryAddress) return true;
                    break;
                }
                case 8: { // sw
                    if ( dstRequest->inst->cirs[0] == srcRequest->changingRegister) return true;
                    if ( dstRequest->inst->cirs[2] == srcRequest->changingRegister) return true;
                    if (dstRequest->savingMemoryAddress == srcRequest->loadingMemoryAddress) return true;
                    // ! sw will have problem with other lw and sw
                    break;
                } 
                case 9: { // j
                    // ! HANDLE ERROR
                    break;
                }
                default: 
                    break;
        }
        return false;
    }

    bool checkDependencies ( int id,Request * r) {
        vector<Request*> * coreQueue  = getQueueOfCore_id (id);
        bool dependent = false;
        for (auto requestInQueue = coreQueue->begin(); requestInQueue != coreQueue->end(); requestInQueue++) {
            switch (r->inst->opID) {
                case 0: // add
                case 1: // sub
                case 2: //mul
                case 3: { // slt 
                    if ( r->inst->cirs[1] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost;
                        dependent = true;
                    }
                    if ( r->inst->cirs[2] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost;    
                        dependent = true;
                    } 
                    //if (dependent) r->cost += 1;
                    break;
                }
                case 4: { // addi 
                    if ( r->inst->cirs[1] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost;
                        dependent = true;
                    }
                    //if (dependent) r->cost += 1;
                    break;
                }
                
                case 5: // bne
                case 6: { //beq
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost;
                        dependent = true;
                    }
                    if ( r->inst->cirs[1] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost; 
                        dependent = true;  
                    }
                    //if (dependent) r->cost += 1;
                    break;
                }
                case 7:  { //lw
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost;
                        dependent = true;
                    }
                    if ( r->inst->cirs[2] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost;  
                        dependent = true;
                    }
                    // ! lw will also have problem with sw 
                    if (r->loadingMemoryAddress == (*requestInQueue)->savingMemoryAddress) {
                        r->cost += (*requestInQueue)->cost;
                        dependent = true;
                    }
                    break;
                }
                case 8: { // sw
                    if ( r->inst->cirs[0] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost;
                        dependent = true;
                    }
                    if ( r->inst->cirs[2] == (*requestInQueue)->changingRegister) {
                        r->cost += (*requestInQueue)->cost; 
                        dependent = true;   
                    }
                    if (r->savingMemoryAddress == (*requestInQueue)->loadingMemoryAddress) {
                        r->cost += (*requestInQueue)->cost; 
                        dependent = true;   
                    }

                    // ! sw will have problem with other lw and sw
                    break;
                } 
                case 9: { // j
                    // ! HANDLE ERROR
                    break;
                }
                default: 
                    break;
            }
        }
        return dependent;
    }

    // * returns true if request was successfully enqueued.
    bool enqueueRequest (int coreId, Request * r) {
        vector<Request*> * coreQueue = getQueueOfCore_id(coreId-1);
        if (coreQueue->size() >= coreQueueLength) return false;
        else {
            coreQueue->push_back(r);
            allCores->at(coreId - 1)->setMinCost (min (r->cost,allCores->at(coreId - 1)->getMinCost()));
        }
        return true;
    }

    

    CORE * selectCore (vector<CORE*> * allCores) {
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

    Request* getMinCostRequest (vector<CORE*> * allCores,int& Delay) { 
        CORE* minCostCore = selectCore(allCores);
        if (minCostCore == nullptr) return nullptr;  
        Request* request = minCostCore->getRequestWithMinCost();
        return request;
    }

    int getDelayOfForwardingRequest (Request* request,vector<CORE*> * allCores, int unitDelay = 1) {
        
    }

    bool dequeueRequest (Request * r, vector<Request*> * coreQueue) {
        bool deleted = false;
        for (auto i = coreQueue->begin(); i != coreQueue->end(); i++) {
            if ( (*i) == r) {
                for (auto j = i+1; j != coreQueue->end(); j++) {
                    if (j != coreQueue->end()) if (isDependent(*i, *j)) (*j)->cost -= (*i)->cost ;
                }
                coreQueue->erase (i);
                deleted = true;
                break;
            }
        }
        return deleted;
    }

    void execute (vector<CORE*> * allCores, int currentClockCycle) {
        if (currentClockCycle > uptoClkCycle) {
            DRAMrequestIssued = false;
            currentRequestInDRAM = nullptr;
        }
        if (currentClockCycle == uptoClkCycle) {
            if (currentRequestInDRAM->inst->opID == 7) {
                cout <<" coreId: "<<currentRequestInDRAM->core_id <<  ": " + CORE::rrmap.at(currentRequestInDRAM->changingRegister) + "=" + to_string(dram->getLoadedValueForLW ()+"\t");
                allCores->at (currentRequestInDRAM->core_id - 1)->getRegisters()->at (currentRequestInDRAM->changingRegister) = dram->getLoadedValueForLW ();
            }
            else if (currentRequestInDRAM->inst->opID == 8) {
                cout <<" coreId: "<<currentRequestInDRAM->core_id << ": memory address " + to_string(currentRequestInDRAM->savingMemoryAddress) + "-" + to_string(currentRequestInDRAM->savingMemoryAddress + 3) + " = " + to_string(dram->getValueToBeStoredForSW()) + "\t";
                dram->setMemory (currentRequestInDRAM->savingMemoryAddress,dram->getValueToBeStoredForSW());
            }
            else allCores->at(currentRequestInDRAM->core_id)->setRuntimeError("Request in DRAM cannot be other than sw and lw");
        }

        if (currentClockCycle < uptoDelayClkCycle) {return;}
        Request* request = getMinCostRequest (allCores,Delay);
        

        if (request == nullptr) return;

        if (request->inst->opID != 7 && request->inst->opID != 8 && request->cost <= 1) {
            dequeueRequest (request,getQueueOfCore_id(request->core_id));

            if  (allCores->at(request->core_id)->getStallingRequest() == request && 
                getQueueOfCore_id(request->core_id)->size() == coreQueueLength) allCores->at(request->core_id)->resume(true);

            else if(allCores->at(request->core_id)->getStallingRequest() == request) allCores->at(request->core_id)->resume(false);

            else allCores->at(request->core_id)->addInFreeBuffer(request);
            return;
        }
        if (request->inst->opID != 7 && request->inst->opID != 8 && request->cost > 1) {
            allCores->at(request->core_id)->setRuntimeError("This should not happen");
            return;
        }

        if (DRAMrequestIssued) {
            // * calculate delay 
            uptoDelayClkCycle = currentClockCycle + 1;
        }
        else {
            DRAMrequestIssued = true;
            
            if (request->inst->opID == 7) {
                cout<<" coreId: "<<currentRequestInDRAM->core_id<<"\t" << "DRAM request issued : lw\t";
                if (dram->getRowBuffer() == DRAM::getRowOfRowBuffer (request->loadingMemoryAddress)) {
                    if (dram->getColBuffer() == DRAM::getColOfRowBuffer (request->loadingMemoryAddress)) {
                        uptoClkCycle = currentClockCycle + 1;
                    }
                    else {
                        uptoClkCycle = currentClockCycle + colAccessDelay; 
                    }   
                }
                else {
                    uptoClkCycle = currentClockCycle + (2 * rowAccessDelay) + colAccessDelay;
                }
                currentRequestInDRAM = request;
                dram->lw (request,allCores->at(request->core_id) );
            }
            else if (request->inst->opID == 8) {
                cout<<" coreId: "<<currentRequestInDRAM->core_id<<"\t" << "DRAM request issued : sw\t";
                if (dram->getRowBuffer() == DRAM::getRowOfRowBuffer (request->savingMemoryAddress)) {
                    if (dram->getColBuffer() == DRAM::getColOfRowBuffer (request->savingMemoryAddress)) {
                        uptoClkCycle = currentClockCycle + 1;
                    }
                    else {
                        uptoClkCycle = currentClockCycle + colAccessDelay; 
                    }   
                }
                else {
                    uptoClkCycle = currentClockCycle + (2 * rowAccessDelay) + colAccessDelay;
                }
                currentRequestInDRAM = request;
                dram->sw (request,allCores->at(request->core_id));
            }
            else allCores->at(request->core_id)->setRuntimeError("not a lw or sw instruction");;
        
        }
        return;
    }



    



};