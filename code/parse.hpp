#include "init.hpp"

#ifndef parse_H
#define init_H 

void RemoveSpaces();
void findRegister (int i);
void removeComma();
void findImmediate();
string findLabel();
void offsetType();
instruction readInst ();
void preprocess ();

#endif 