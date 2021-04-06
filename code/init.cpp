#include "init.hpp"

//Max int val: 	2147483647

vector<int> CIRS (4,0);   //current instruction register set
int sizeOfData = 0;
string error1 = "";
vector<string> error;
vector<int> data123 (1<<18,0);    // all memory addresses
vector<int> numOfInst (10,0);   // which instruction is called how many times
int numberofInst=0;

int numClockCycles = 0;
unordered_map <string,int> rmap;
unordered_map <int, string> rrmap; // reverse register map
vector<string> inputprogram;
vector<instruction> iset;
vector<int> registers(32,0);
string file , outfile = "output.txt";
unordered_map<string,int> lableTable; //which lable is present on which instruction number
unordered_map <int,string> uniqueLabelID;

int numberOfLine = 0 , currentInstNum = 0, maxInstructions = 0 ;
string currentInst;
int labelNumber = 0;
vector<string> myset = {"add","sub","mul","slt","addi","bne","beq","lw","sw","j"};
//                        0     1     2     3      4     5     6     7    8   9       opIDs    //
//                        R     R     R     R     I      I     I    

int pc = 1;   // program counter  //



//  IMPORTANT DECLARATIONS FOR MINOR EXAM

bool part2enabled = false;
const int numDramRows = 1024, numDramCols = 1024;
const int  actualNumDramCols = 256;
int ROW_ACCESS_DELAY = 0, COL_ACCESS_DELAY = 0;
int numRowBufferUpdates = 0;
int currentRowInRowBuffer = -1;
int uptoClkCyc = 0;
int lwInstRegisterID = -1;
int blockRegisterID = -1;

int swInstMemAdd = -1;
int blockMemoryAdd = -1;
vector<string> exectutionOutput;
bool switchOnBranch = false;
bool DRAMrequestIssued = false;

// DECLARATIONS FOR MINOR EXAM END
// for every instruction
unordered_set<int> cannotUseRegisters;
unordered_set<int> cannotChangeRegisters;
unordered_set<int> cannotUseMemory;
unordered_set<int> cannotChangeMemory;
// vector[0] = cannotUseRegisters;
// vector[1] = cannotChangeRegisters;
// vector[2] = cannotUseMemory;
// vector[3] = cannotChangeMemory;

unordered_map<int,queue<int>> uBlockRegToInstAddQueue;
unordered_map<int,queue<int>> uBlockMemoryToInstAddQueue;
unordered_map<int,queue<int>> cBlockRegToInstAddQueue;
unordered_map<int,queue<int>> cBlockMemoryToInstAddQueue;

map<int,vector<unordered_set<int>>> InstAddToBlocks;

unordered_map<int,int> InstAddToFreq;
unordered_map<int,set<int>> rowToInsSet;
// global
multiset<int> cannotUseRegistersG;
multiset<int> cannotChangeRegistersG;
multiset<int> cannotUseMemoryG;
multiset<int> cannotChangeMemoryG;




// Used to read raw input MIPS file and store individual lines in inputProgram.
void readFile(string file)
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

// Used in Assignment - 3 to print state of registers after each instruction
void printVector(ofstream &out)
{
    cout << "[";
    out << "[";
    for (int i = 0; i < 31; i++)
    {
        if (registers[rmap["$zero"]] != 0)
        {
            error1 = "Error: Register Zero Cannot Be Modified";
            return;
        }

        string s = "$";
        s = s + to_string(i);
        cout << s << ": " << hex << registers[rmap[s]] << dec << ",";
        out << s << ": " << hex << registers[rmap[s]] << dec << ",";
    }

    string s = "$";
    s = s + to_string(31);
    cout << s << ": " << hex << registers[rmap[s]] << dec << "]" << endl;
    out << s << ": " << registers[rmap[s]] << dec << "]" << endl;
    return;
}

void initialize_registers()
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

pair<int, int> getActualRowColFromAddress(int address)
{ // address supplied should be a multiple of 4
    pair<int, int> mypair;
    if (address < 0)
    {
        mypair.first = -1;
        mypair.second = -1;
        return mypair;
    }
    else if (address % 4 != 0)
    {
        mypair.first = -2;
        mypair.second = -2;
        return mypair;
    }
    else if (address > (numDramRows * numDramCols - 4))
    { //(1024*1024 - 4) last accessible address
        mypair.first = -3;
        mypair.second = -3;
        return mypair;
    }
    mypair.first = address / numDramRows;                     // row of dram
    mypair.second = (address - (mypair.first) * numDramRows); // actual col of dram
    return mypair;
}

int getRowOfRowBuffer(int address)
{
    pair<int, int> mypair = getActualRowColFromAddress(address);
    return mypair.first;
}
int getColOfRowBuffer(int address)
{
    pair<int, int> mypair = getActualRowColFromAddress(address);
    return mypair.second;
}