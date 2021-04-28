#include "main.hpp"
#include "Instruction.hpp"
#include "Request.hpp"
class CORE {

private:
    int core_id;
    static unordered_map <string,int> rmap;
    static unordered_map <int, string> rrmap;
    string error1 = "";
    vector<string> error;
    vector<int> CIRS;   //current instruction register set
    int sizeOfData = 0;
    //vector<int> data123 (1<<18,0);    // all memory addresses
    vector<int> numOfInst;   // which instruction is called how many times
    int numberofInst=0;

    //int numClockCycles = 0;

    vector<instruction> iset;
    vector<int> registers;
    string outfile = "output.txt";
    unordered_map<string,int> lableTable; //which lable is present on which instruction number
    unordered_map <int,string> uniqueLabelID;

    int numberOfLine = 0 , currentInstNum = 0, maxInstructions = 0 ;
    string currentInst;
    int labelNumber = 0;
    vector<string> myset = {"add","sub","mul","slt","addi","bne","beq","lw","sw","j"};
    //                        0     1     2     3      4     5     6     7    8   9       opIDs    //
    //                        R     R     R     R     I      I     I    

    int pc = 1;
    

    string path;
    vector<string> inputprogram;
    int numberOfLine = 0 , currentInstNum = 0, maxInstructions = 0 ;
    
    string currentInst;

    void RemoveSpaces();
    void findRegister(int i);
    void removeComma();
    void findImmediate();
    string findLabel();
    void offsetType();
    instruction readInst();
    void preprocess();
    
    

    instruction readInst ();
    void preprocess ();
    
    static void initialize_registers();

    vector<Request*> requestQueue;
    int minCost;

    int findMinCost () {
        for (auto i : requestQueue) {
            minCost = min (minCost, i->cost);
        }
    }



public:
    static void initAllCores () { initialize_registers(); }
    void  lexFile(string file);

    CORE (string fileName, int id)
    {
        core_id = id;
        path = fileName;    
        for (int i = 0; i < 4; i++) {
            CIRS.push_back(0);
        }
        for (int i = 0; i < 10; i++) {
            numOfInst.push_back(0);
        }
        for (int i = 0; i < 32; i++) {
            registers.push_back(0);
        }
        minCost = INT_MAX;

        lexFile (path); // * Now our inputprogram is ready to parse
        preprocess();   // * Now our iset is ready
    }


    int getMinCost () {return minCost;}
    ~CORE();



    





};
