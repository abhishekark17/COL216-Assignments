#pragma once
#ifndef CPU_HPP
#define CPU_HPP
#include "Core.hpp"
#include "main.hpp"
#include "output.hpp"
class CPU {
private:
    int numCores;
    int curClockCycle;
    int maxClockCycle;
    vector<CORE*> * allCores;
    string folderPath;
    string folderPathOutput;
    int ROW_ACCESS_DELAY;
    int COL_ACCESS_DELAY;

    DRAM *dram;
    MRM *memoryRequestManager;
    OutputHandler * outputHandler;
public: 
    CPU (int n,int m,  string fpi, string fpo,int rad,int cad, string outputFolderPath);

     void exit ();

     void run ();
    CORE* getCoreFromID (int id) {
        return allCores->at (id - 1);
    }
     int getNumCores();
};

#endif