#include "main.hpp"

class DRAM {
private:
    vector<int> * memory; 
    static const int numDramRows = 1024, numDramCols = 1024;
    int currentRow, currentCol;
    static pair<int, int> getActualRowColFromAddress(int address);
public:
    static int getRowOfRowBuffer(int address);
    static int getColOfRowBuffer(int address);
    


    DRAM () {
        memory = new vector<int> (1<<18,0);
        currentRow = -1, currentCol = -1;
    }
    void setMemory (int address, int value) {
        memory->at(address) = value;
    }
    int getMemory (int address) {
        return memory->at(address);
    }
    void setRowBuffer (int row) {currentRow = row;}
    int getRowBuffer () {return currentRow;}
    void setColBuffer (int col) {currentCol = col;}
    int getColBuffer () {return currentCol;}
    
    ~DRAM();
    

};