#include "executeInst.hpp"

void add(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] + registers[cirs[2]];
    return;
}
void sub(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] - registers[cirs[2]];
    return;
}
void mul(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] * registers[cirs[2]];
    return;
}
void addi(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] + cirs[2];
    return;
}
void bne(vector<int> &cirs, string &label)
{
    if (lableTable.find(label) == lableTable.end())
    {
        error1 = "Error: Label Not Found";
        return;
    }
    if (registers[cirs[0]] != registers[cirs[1]])
    {
        if (lableTable.find(label) != lableTable.end())
        {
            pc = lableTable[label];
            switchOnBranch = true;
        }
        else
        {
            error1 = "Error: Label Not Found";
            return;
        }
    }
    else
        switchOnBranch = false;
    return;
}
void beq(vector<int> &cirs, string &label)
{
    if (lableTable.find(label) == lableTable.end())
    {
        error1 = "Error: Label Not Found";
        return;
    }

    if (registers[cirs[0]] == registers[cirs[1]])
    {
        if (lableTable.find(label) != lableTable.end())
        {
            pc = lableTable[label];
            switchOnBranch = true;
        }
        else
        {
            error1 = "Error: Label Not Found";
            return;
        }
    }
    else
        switchOnBranch = false;
    return;
}

void slt(vector<int> &cirs)
{
    if (registers[cirs[1]] < registers[cirs[2]])
        registers[cirs[0]] = 1;
    else
        registers[cirs[0]] = 0;
    return;
}

void lw(vector<int> &cirs)
{
    int offset = cirs[1];
    int typeOfarg = cirs[3];
    int address;

    if (typeOfarg == 0)
        address = offset + registers[cirs[2]];
    else
        address = offset + cirs[2];
    if (address < 4 * numberofInst || address > 1048576)
    {
        error1 = "Error: Memory Address not accessible";
        return;
    }
    else if (address % 4 != 0)
    {
        error1 = "Error: invalid memory location";
        return;
    }
    else
    {
        address /= 4;
        address -= numberofInst;
    }
    registers[cirs[0]] = data123[address];
    return;
}

void sw(vector<int> &cirs)
{
    int offset = cirs[1];
    int typeOfarg = cirs[3];
    int address;
    if (typeOfarg == 0)
        address = offset + registers[cirs[2]];
    else
        address = offset + cirs[2];

    if (address < 4 * numberofInst || address > 1048576)
    {
        error1 = "Error: Memory Address not accessible";
        return;
    }
    else if (address % 4 != 0)
    {
        error1 = "Error: invalid memory location";
        return;
    }
    else
    {
        address /= 4;
        address -= numberofInst;
    }
    data123[address] = registers[cirs[0]];
    return;
}

void j(vector<int> &cirs, string &label)
{
    pc = lableTable[label];
    return;
}