#include "executeInst.hpp"

#ifndef common_H
#define common_H 

bool ifLW(instruction &curInst, int address);
bool ifSW(instruction &curInst, int address);
void eraseIfP_URG(int reg);
void eraseIfP_CRG(int reg);
void eraseIfP_UMG(int mem);
void eraseIfP_CMG(int mem);
void eraseIfP_LW(instruction curInst);
void eraseIfP_SW(instruction curInst);

int getAddress(instruction &curInst);
void intializerowToInsSet();
void removeInstFromDeque(deque<int> &mydeq, int insAddr);
void eraseAllBlocks(int insAddr);
#endif 