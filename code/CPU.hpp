#pragma once
#include "Core.hpp"
#include "main.hpp"
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
public: 
    CPU (int n,int m,  string fpi, string fpo,int rad,int cad);

     void exit ();

     void run ();

     int getNumCores();
};