

#ifndef MRM_HPP
#define MRM_HPP

#include "Core.hpp"
#include "DRAM.hpp"
#include "Request.hpp"
#include "main.hpp"
#include "output.hpp"
class CORE;
class Request;
class DRAM;
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
    OutputHandler * handleOutput;
public:
    MRM (DRAM * d,vector<CORE*> * ac, int nc, int rad, int cad,OutputHandler * ho, int cql);

    vector<Request*> * getQueueOfCore_id (int id);

    // * NOTE: dstRequest will be ahead of srcRequest
    bool isDependent (Request* srcRequest, Request * dstRequest);
    
    // * This function updates the cost of input request depending upon what all instructions it is dependent on and returns if it is dependent or not
    // * let us keep the default cost of instruction as zero
    bool checkDependencies ( int id,Request * r);
    pair<Request *, int> getMinCostRequest (vector<Request*> * coreQueue);

    // * returns true if request was successfully enqueued.
    bool enqueueRequest (int coreId, Request * r);
    bool dequeueRequest (Request * r, vector<Request*> * coreQueue);

    CORE * selectCore (vector<CORE*> * allCores);
    Request* getMinCostRequest (vector<CORE*> * allCores,int& Delay);
    int getDelayOfForwardingRequest (Request* request,vector<CORE*> * allCores, int unitDelay = 1);

    void execute (vector<CORE*> * allCores, int currentClockCycle);
};

#endif