#include "output.hpp"

OutputHandler::OutputHandler (int numberOfCores, string folderPath) {
    numCores = numberOfCores;
    allStreams = new vector<ofstream> (numberOfCores);
    allCoreOutputs = new vector<string> (numberOfCores, "");
    allCoreNumOfInst = new vector<vector<int>> (numberOfCores, vector<int> ());
    totalInstructionCount = new vector<int> (numberOfCores, 0);

    DramOutput = "";

    for (int i = 1; i <= numberOfCores; i++) {
        allStreams->at (i-1).open (folderPath + "/" + to_string(i) + "o.txt");
    }
    DRAMStream.open (folderPath + "/DramOutput.txt");

    allCoreNumOfInst = new vector<vector<int>> (numCores,vector<int> ());
    
}

void OutputHandler::addOutputForCore (int core_id, string output) {
    allCoreOutputs->at (core_id - 1) = output;
}

void OutputHandler::appendOutputForCore (int core_id, string output) {
    allCoreOutputs->at (core_id - 1) = output + allCoreOutputs->at (core_id - 1);
}

void OutputHandler::addDramOutput (string o) {
    DramOutput = o + DramOutput;
}

void OutputHandler::setDramOutput (string s) {
    DramOutput = s;
}
string OutputHandler::getDramOutput () {return DramOutput;}

void OutputHandler::printAll (int cycleNum) {
    for (int i = 0; i <numCores; i++) {
        allStreams->at (i) << "Cycle: " << cycleNum<<"\t" << allCoreOutputs->at (i) << endl;
    }

    DRAMStream << "Cycle: " << cycleNum << "\t" << DramOutput << endl;
    for (int i = 0; i <numCores; i++) {
        allCoreOutputs->at (i) = "";
    }
    DramOutput = "";
    }

void OutputHandler::printMetaData (vector<string> * dramMetaData, int numLw, int numSw) {
    for (int i = 0; i <numCores; i++) {
        allStreams->at (i) << endl << endl << "META DATA" << endl;
        allStreams->at (i) << "[";
        for (int j = 0; j < allCoreNumOfInst->at (i).size ()- 1; j++) {
            totalInstructionCount->at (i) += allCoreNumOfInst->at (i)[j];
            allStreams->at (i) << myset[j] << ": " << allCoreNumOfInst->at(i)[j] << ",";
        }
        totalInstructionCount->at (i) += allCoreNumOfInst->at (i)[allCoreNumOfInst->at (i).size ()- 1];
        allStreams->at (i) << myset[allCoreNumOfInst->at (i).size ()- 1] << ": " << allCoreNumOfInst->at(i)[allCoreNumOfInst->at (i).size ()- 1] << "]";

        allStreams->at (i) << endl << "TOTAL NUMBER OF INSTRUCTIONS EXECUTED := " << totalInstructionCount->at (i) << endl;
    }

    DRAMStream << endl << "META DATA" << endl;
    DRAMStream << "Number of LW instructions executed by DRAM := " << numLw << endl;
    DRAMStream << "Number of SW instructions executed by DRAM := " << numSw << endl << endl;
    DRAMStream << endl << "MEMORY LOCATIONS WHICH CHANGED AFTER EXECUTION" << endl;
    
    for (string x : (*dramMetaData)) {
        DRAMStream << x << endl;
    }

    int allCoresInstCount = 0;
    for (int i : (*totalInstructionCount)) allCoresInstCount += i;
    cout << endl << "Total instructions executed := " << allCoresInstCount << endl << endl;
}   

void OutputHandler::updateNumOfInstForCore ( int core_id,vector<int>* numOfInst) {
    allCoreNumOfInst->at(core_id - 1) = (*numOfInst);
}

void OutputHandler::close () {
    for (int i = 0; i < numCores; i++) {
        allStreams->at (i).close ();
    }
    DRAMStream.close ();
}