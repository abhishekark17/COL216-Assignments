#include "output.hpp"

OutputHandler::OutputHandler (int numberOfCores, string folderPath) {
    numCores = numberOfCores;
    allStreams = new vector<ofstream> (numberOfCores);
    allCoreOutputs = new vector<string> (numberOfCores, "");

    for (int i = 1; i <= numberOfCores; i++) {
        allStreams->at (i-1).open (folderPath + "/" + to_string(i) + "o.txt");
    }
    DRAMStream.open (folderPath + "/DramOutput.txt");
    
    DramOutput = "";
}

void OutputHandler::addOutputForCore (int core_id, string output) {
    allCoreOutputs->at (core_id - 1) = output;
}

void OutputHandler::appendOutputForCore (int core_id, string output) {
    allCoreOutputs->at (core_id - 1) = output + allCoreOutputs->at (core_id - 1);
}

void OutputHandler::addDramOutput (string o) {
    DramOutput = o;
}

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

void OutputHandler::close () {
    for (int i = 0; i < numCores; i++) {
        allStreams->at (i).close ();
    }
    DRAMStream.close ();
}