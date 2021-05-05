#ifndef OUTPUT_HPP
#define OUTPUT_HPP
#include "main.hpp"

class OutputHandler {

private:
    int numCores;
    vector<ofstream> *allStreams;
    vector<string> *allCoreOutputs;
    vector<vector<int>> * allCoreNumOfInst;
    vector<int> * totalInstructionCount; // for each core
    ofstream DRAMStream;
    string DramOutput;

    vector<string> myset = {"add","sub","mul","slt","addi","bne","beq","lw","sw","j"};

public:
    OutputHandler (int numberOfCores, string folderPath);

    void addOutputForCore (int core_id, string output);

    void addDramOutput (string o);
    string getDramOutput ();
    void setDramOutput (string s);
    void appendOutputForCore (int core_id, string output);
    void printAll (int cycleNum);
    void printMetaData (vector<string> * dramMetaData, int numLw, int numSw);

    void updateNumOfInstForCore ( int core_id,vector<int>* numOfInst);

    void close ();
};

#endif