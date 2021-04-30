

#ifndef Core_HPP
#define Core_HPP

#include "MRM.hpp"
#include "main.hpp"
#include "Instruction.hpp"
#include "Request.hpp"
/* *
*CIRS DEFINITION FOR VARIOUS INSTRUCTIONS:
*add,sub,mul, slt := [register1 ,register2 ,register3 , NotUsed]
*addi := [register1,register2,immediateValue,NotUsed]
*lw, sw := [register1, offset , last register , 0] // 0 if register is there as last arguement inside parenthesis
       := [register1, offset ,address , 1]  // 1 if address given as argument inside braces
*bne, beq := [register1, register2, NotUsed, NotUsed]
*j  := [NotUsed, NotUsed, NotUsed, NotUsed]
*/

class MRM;
class CORE {

private:
    int core_id;
    string runtimeError;
    vector<string> error;
    vector<int> CIRS;   //current instruction register set
    int sizeOfData;
    vector<int> numOfInst;   // which instruction is called how many times
    int numberofInst;
    int numCores;
    int Dramcols;
    int Dramrows;
    int offsetOfCore;
    vector<int> * registers;
    unordered_map<string,int> lableTable; //which lable is present on which instruction number
    unordered_map <int,string> uniqueLabelID;
    int numberOfLine, currentInstNum, maxInstructions;
    string currentInst;
    int labelNumber;
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
    void printIset ();
    
    vector<instruction *> * iset;
    
    void initialize_registers();

    vector<Request*> * requestQueue;
    int minCost;
    Request * minCostRequest;
    vector<Request*> * freeBuffer;

    int findMinCost () {
        for (auto i = requestQueue->begin();i!=requestQueue->end();i++) {
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
    unordered_map <string,int> rmap;
    unordered_map <int, string> rrmap;
    
    void  lexFile(string file);

    Request * getStallingRequest () { return stallingRequest; }

    bool writingFromDRAM;

    CORE (string inFileName, int id, string outFileName,int nc,int DramCols,int DramRows);

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


    void run (MRM *memoryRequestManager);

    void addInFreeBuffer (Request * request) {
        freeBuffer->push_back(request);
    }

    void smoothExit () {
        cout << "smoothExit called " << core_id<< endl;
    }

    bool isRunnable () {return pc <= iset->size();} 

    ~CORE();
};




#endif