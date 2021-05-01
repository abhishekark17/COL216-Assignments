#ifndef OUTPUT_HPP
#define OUTPUT_HPP
#include "main.hpp"

class OutputHandler {

private:
    int numCores;
    vector<ofstream> *allStreams;
    vector<string> *allCoreOutputs;
    ofstream DRAMStream;
    string DramOutput;

public:
    OutputHandler (int numberOfCores, string folderPath);

    void addOutputForCore (int core_id, string output);

    void addDramOutput (string o);
    void appendOutputForCore (int core_id, string output);
    void printAll (int cycleNum);

     void close ();
};

#endif