#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <unordered_set>

#include <queue>
#ifndef init_H
#define init_H 

using namespace std;

/*
CIRS DEFINITION FOR VARIOUS INSTRUCTIONS:
add,sub,mul, slt := [register1 ,register2 ,register3 , NotUsed]
addi := [register1,register2,immediateValue,NotUsed]
lw, sw := [register1, offset , last register , 0] // 0 if register is there as last arguement inside parenthesis
       := [register1, offset ,address , 1]  // 1 if address given as argument inside braces
bne, beq := [register1, register2, NotUsed, NotUsed]
j  := [NotUsed, NotUsed, NotUsed, NotUsed]
*/

// extern := implies that that variable is a global variable and can be accessed from any file.


extern vector<int> CIRS;   //current instruction register set
extern int sizeOfData;
extern string error1;
extern vector<string> error;
extern vector<int> data123;    // all memory addresses
extern vector<int> numOfInst;   // which instruction is called how many times
extern int numberofInst;


class instruction {
  public:
    vector<int> cirs;
    int opID;
    string label="";
    instruction (vector<int> CIRS , int id) {
        cirs = CIRS;
        opID = id;
    }
    instruction (vector<int> CIRS , int id,string str) {
        cirs = CIRS;
        opID = id;
        label=str;
    }
    instruction () {
        cirs = {-1,-1,-1,-1};
        opID = -1;
    }
};

extern int numClockCycles;
extern unordered_map <string,int> rmap;
extern unordered_map <int, string> rrmap; // reverse register map
extern vector<string> inputprogram;
extern vector<instruction> iset;
extern vector<int> registers;
extern string file , outfile;
extern unordered_map<string,int> lableTable; //which lable is present on which instruction number
extern unordered_map <int,string> uniqueLabelID;

extern int numberOfLine , currentInstNum, maxInstructions;
extern string currentInst;
extern int labelNumber;
extern vector<string> myset;
extern int pc;   // program counter  //



//  IMPORTANT DECLARATIONS FOR MINOR EXAM

extern unordered_set<int> cannotUseRegisters;
extern unordered_set<int> cannotChangeRegisters;
extern unordered_set<int> cannotUseMemory;
extern unordered_set<int> cannotChangeMemory;

extern unordered_map<int,int> BlockRegToInstAdd;
extern unordered_map<int,int> BlockMemoryToInstAdd;
extern unordered_map<int,vector<unordered_set<int>>> InstAddToBlocks;


// global
extern unordered_set<int> cannotUseRegistersG;
extern unordered_set<int> cannotChangeRegistersG;
extern unordered_set<int> cannotUseMemoryG;
extern unordered_set<int> cannotChangeMemoryG;

extern bool part2enabled;
extern const int numDramRows, numDramCols;
extern const int  actualNumDramCols;
extern int ROW_ACCESS_DELAY, COL_ACCESS_DELAY;
extern int numRowBufferUpdates;
extern int currentRowInRowBuffer;
extern int uptoClkCyc;
extern int lwInstRegisterID;
extern int blockRegisterID;

extern int swInstMemAdd;
extern int blockMemoryAdd;

extern vector<string> exectutionOutput;
extern bool switchOnBranch;
extern bool DRAMrequestIssued;


pair<int,int> getActualRowColFromAddress (int address);
int getRowOfRowBuffer (int address);
int getColOfRowBuffer (int address);

// MINOR EXAM DECLARATIONS END




void initialize_registers ();
void printVector(ofstream& out);
void readFile (string file);


#endif