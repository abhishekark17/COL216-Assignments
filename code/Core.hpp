

#ifndef Core_HPP
#define Core_HPP

#include "MRM.hpp"
#include "main.hpp"
#include "Instruction.hpp"
#include "Request.hpp"
#include "output.hpp"
#include "DRAM.hpp"
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

class DRAM;

class Request;
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
    
    vector<instruction> iset;
    
    void initialize_registers();

    int minCost;
    Request * minCostRequest;
    vector<Request*> * freeBuffer;

    void add(vector<int> &cirs);
    void sub(vector<int> &cirs);
    void mul(vector<int> &cirs);
    void slt(vector<int> &cirs);
    void addi(vector<int> &cirs);
    void bne(vector<int> &cirs, string &label);
    void beq(vector<int> &cirs, string &label);
    //void lw(vector<int> &cirs);
    //void sw(vector<int> &cirs);
    void j(vector<int> &cirs, string &label);

    bool stalled;
    
    Request * stallingRequest;

    bool switchOnBranch;
    OutputHandler * handleOutput;

    int rowAccessDelay, colAccessDelay;
    bool working;
    bool hasSyntaxError;
    bool hasRuntimeError;
    bool isFromFreeBuffer;

    instruction postponedInstruction;

public:
    bool stallIfFull;
    int getCoreId () { return core_id;}
    unordered_map <string,int> rmap;
    unordered_map <int, string> rrmap;
    
    void  lexFile(string file);
    bool isWorking ();

    void setMinCostRequest (Request *r);
    Request* getMinCostRequest ();
    Request * getStallingRequest ();

    bool writingFromDRAM;

    CORE (string inFileName, int id, string outFileName,int nc,int DramCols,int DramRows,OutputHandler * ho,int rowAccessDelay,int ColAccessDelay);

    Request* getRequestWithMinCost ();

    void setRuntimeError (string s);
    string getRuntimeError ();

    int getMinCost ();
    void setMinCost (int c);
    vector<int> * getRegisters ();

    void stall (Request * request, bool sif);
    void resume (bool sif);
    void run (MRM *memoryRequestManager);
    void addInFreeBuffer (Request * request);
    void smoothExit ();
    bool isRunnable ();

    void updateNumOfInst (int instOpId);
    vector<int> * getNumOfInst ();
    
    bool isStalled ();
    ~CORE();
};




#endif