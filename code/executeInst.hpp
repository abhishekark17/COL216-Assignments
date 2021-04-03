#include "init.hpp"


#ifndef executeInst_H
#define executeInst_H 

void add (vector<int>& cirs);
void sub (vector<int>& cirs);
void mul (vector<int>& cirs);
void addi (vector<int>& cirs);
void bne (vector<int>& cirs,string& label);
void beq (vector<int>& cirs,string& label);
void slt (vector<int>& cirs);
void lw (vector<int>& cirs);
void sw (vector<int>& cirs);
void j (vector<int>& cirs,string& label);

#endif 