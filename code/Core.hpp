#include "main.hpp"
#include "Instruction.hpp"
#include "Request.hpp"
#include "MRM.hpp"
/* *
*CIRS DEFINITION FOR VARIOUS INSTRUCTIONS:
*add,sub,mul, slt := [register1 ,register2 ,register3 , NotUsed]
*addi := [register1,register2,immediateValue,NotUsed]
*lw, sw := [register1, offset , last register , 0] // 0 if register is there as last arguement inside parenthesis
       := [register1, offset ,address , 1]  // 1 if address given as argument inside braces
*bne, beq := [register1, register2, NotUsed, NotUsed]
*j  := [NotUsed, NotUsed, NotUsed, NotUsed]
*/

class CORE {

private:
    int core_id;
    static unordered_map <string,int> rmap;
    
    string runtimeError = "";
    vector<string> error;
    vector<int> CIRS;   //current instruction register set
    int sizeOfData = 0;
    //vector<int> data123 (1<<18,0);    // all memory addresses
    vector<int> numOfInst;   // which instruction is called how many times
    int numberofInst=0;
    int numCores;
    int Dramcols;
    int Dramrows;
    int offsetOfCore;

    //int numClockCycles = 0;

    
    vector<int> * registers;
    
    unordered_map<string,int> lableTable; //which lable is present on which instruction number
    unordered_map <int,string> uniqueLabelID;

    int numberOfLine = 0 , currentInstNum = 0, maxInstructions = 0 ;
    string currentInst;
    int labelNumber = 0;
    vector<string> myset = {"add","sub","mul","slt","addi","bne","beq","lw","sw","j"};
    //                        0     1     2     3      4     5     6     7    8   9       opIDs    //
    //                        R     R     R     R     I      I     I    

    int pc;
    

    string inputFileName, outputFileName;
    vector<string> inputprogram;

    void RemoveSpaces();
    void findRegister(int i);
    void removeComma();
    void findImmediate();
    string findLabel();
    void offsetType();
    instruction readInst();
    void preprocess();
    
    vector<instruction *> * iset;

    instruction readInst ();
    void preprocess ();
    
    static void initialize_registers();

    vector<Request*> * requestQueue;
    int minCost;
    Request * minCostRequest;

    vector<Request*> * freeBuffer;

    int findMinCost () {
        for (auto i = requestQueue->beign();i!=requestQueue->end();i++) {
            if(minCost<((*i)->cost)) {
                minCost=(*i)->cost;
                minCostRequest = (*i);
            }
        }
        return minCost;
    }

    void add(vector<int> &cirs);
    void sub(vector<int> &cirs);
    void mul(vector<int> &cirs);
    void slt(vector<int> &cirs);
    void addi(vector<int> &cirs);
    void bne(vector<int> &cirs, string &label);
    void beq(vector<int> &cirs, string &label);
    void lw(vector<int> &cirs);
    void sw(vector<int> &cirs);
    void j(vector<int> &cirs, string &label);

    bool stalled;
    bool stallIfFull;
    Request * stallingRequest;

    bool switchOnBranch;

public:
    static unordered_map <int, string> rrmap;
    static void initAllCores () { initialize_registers(); }
    void  lexFile(string file);

    Request * getStallingRequest () { return stallingRequest; }

    bool writingFromDRAM;

    CORE (string inFileName, int id, string outFileName,int nc,int DramCols,int DramRows)
    {
        core_id = id;
        inputFileName = inFileName;   
        outputFileName = outFileName;
        numCores = nc;
        Dramcols = DramCols;
        Dramrows = DramRows;
        writingFromDRAM = false;
        switchOnBranch = false;
        offsetOfCore = ((id-1)*((Dramcols)/numCores))*Dramrows;

        for (int i = 0; i < 4; i++) {
            CIRS.push_back(0);
        }
        for (int i = 0; i < 10; i++) {
            numOfInst.push_back(0);
        }
        for (int i = 0; i < 32; i++) {
            registers->push_back(0);
        }
        minCost = INT_MAX;

        lexFile (inputFileName); // * Now our inputprogram is ready to parse
        preprocess();   // * Now our iset is ready

        pc = 1;
        stalled = false;
        stallIfFull = false;
        stallingRequest = nullptr;
    }

    Request* getRequestWithMinCost () { return minCostRequest; }
    void setRuntimeError (string s) {runtimeError = s;}
    string getRuntimeError () {return runtimeError;}

    int getMinCost () {return minCost;}
    void setMinCost (int c) {minCost = c;}

    vector<int> * getRegisters () {return registers;}
    void stall (Request * request, bool sif) {
        stalled = true;
        stallIfFull = sif;
        cout <<" coreId: \t"<<core_id<<"STALLED";
        stallingRequest = request;
    }

    void resume (bool sif) {
        stalled = false;
        if (sif) stallIfFull = false;

    }


    void run (MRM *memoryRequestManager) {
        if (stalled) return;
        if (pc > maxInstructions) smoothExit();

        instruction* currentInstruction;
        if (stallingRequest != nullptr) currentInstruction = stallingRequest->inst;
        else {
            if (freeBuffer->size() > 0) {
                currentInstruction = freeBuffer->at(0)->inst;
                freeBuffer->erase (freeBuffer->begin());
            }
            else {
                currentInstruction = iset->at (pc - 1);
                pc++;
            }
        }
        
        switch (currentInstruction->opID) {
            case 0: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                if (dependent) {
                    bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                    if (!enqueued) stall(request,true);
                    
                }
                else {
                    add (currentInstruction->cirs);
                    cout<<" coreId: "<<core_id<<"\t"<<" Instruction: add (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers[currInst.cirs[0]]);
                    numOfInst[0]++;
                }
                break;
            }
            case 1: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                if (dependent) {
                    bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                    if (!enqueued) stall(request,true);
                }
                else {
                    sub (currentInstruction->cirs);
                    cout<<" coreId: "<<core_id<<"\t"<<" Instruction: sub (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers[currentInstruction->cirs[0]]);
                    numOfInst[1]++;
                }
                break;
            }
            case 2: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                if (dependent) {
                    bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                    if (!enqueued) stall(request,true);
                }
                else {
                    mul (currentInstruction->cirs);
                    cout<<" coreId: "<<"\t"core_id<<" Instruction: mul (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers[currentInstruction->cirs[0]]);
                    numOfInst[2]++;
                }
                break;
            }
            case 3: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                if (dependent) {
                    bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                    if (!enqueued) stall(request,true);
                }
                else {
                    slt (currentInstruction->cirs);
                    cout<<" coreId: "<<core_id<<"\t"<< " Instruction: slt (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + rrmap.at(currInst.cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers[currentInstruction->cirs[0]]);
                    numOfInst[3]++;
                }
                break;
            }
            case 4: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                if (dependent) {
                    bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                    if (!enqueued) stall(request,true);
                }
                else {
                    addi (currentInstruction->cirs);
                    cout<<" coreId: "<<core_id<<"\t"<<"Instruction: addi (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + to_string(currentInstruction->cirs[2]) + ")" + ": " + rrmap.at(currentInstruction->cirs[0]) + "=" + to_string(registers[currentInstruction->cirs[0]]);
                    numOfInst[4]++;
                }
                break;
            }
            case 5: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                if (dependent) {
                    bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                    if (!enqueued) stall(request,true);
                    else stall(request,false);
                }
                else {
                    bne (currentInstruction->cirs, currentInstruction->label);
                    cout<<" coreId: "<<core_id<<"\t"<<": Instruction bne (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + currentInstruction->label + "," + to_string(switchOnBranch) + ")";
                    numOfInst[5]++;
                }
                break;
            }
            case 6: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                if (dependent) {
                    bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                    if (!enqueued) stall(request,true);
                    else stall(request,false);
                }
                else {
                    beq (currentInstruction->cirs, currentInstruction->label);
                    cout<<" coreId: "<<core_id<<"\t"<<": Instruction beq (" + rrmap.at(currentInstruction->cirs[0]) + "," + rrmap.at(currentInstruction->cirs[1]) + "," + currentInstruction->label + "," + to_string(switchOnBranch) + ")";
                    numOfInst[6]++;

                }
                break;
            }
            case 7: 
            case 8: {
                Request * request = new Request(0,core_id,currentInstruction);
                bool dependent = memoryRequestManager->checkDependencies(core_id, request);
                bool enqueued = memoryRequestManager->enqueueRequest (core_id,request);
                if (!enqueued) stall(request,true);
                break;
            }
            case 9: {
                j (currentInstruction->cirs,currentInstruction->label);

                numOfInst[9]++;
                break;
            }
            default:

        }

    }

    void addInFreeBuffer (Request * request) {
        freeBuffer->push_back(request);
    }

    void smoothExit () {

    }

    ~CORE();
};
