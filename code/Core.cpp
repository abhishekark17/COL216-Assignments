#include "Core.hpp"    
 void CORE::lexFile(string file)
    {
        ifstream ifs(file);
        string myline;
        while (ifs.good())
        {
            getline(ifs, myline);
            numberOfLine++;
            inputprogram.push_back(myline);
        }
        ifs.close();
        return;
    }   
 void CORE::initialize_registers()
{
    rmap["$zero"] = 0;
    rmap["$0"] = 0;
    rmap["$at"] = 1;
    rmap["$1"] = 1;
    rmap["$v0"] = 2;
    rmap["$2"] = 2;
    rmap["$v1"] = 3;
    rmap["$3"] = 3;
    rmap["$a0"] = 4;
    rmap["$4"] = 4;
    rmap["$a1"] = 5;
    rmap["$5"] = 5;
    rmap["$a2"] = 6;
    rmap["$6"] = 6;
    rmap["$a3"] = 7;
    rmap["$7"] = 7;
    rmap["$t0"] = 8;
    rmap["$8"] = 8;
    rmap["$t1"] = 9;
    rmap["$9"] = 9;
    rmap["$t2"] = 10;
    rmap["$10"] = 10;
    rmap["$t3"] = 11;
    rmap["$11"] = 11;
    rmap["$t4"] = 12;
    rmap["$12"] = 12;
    rmap["$t5"] = 13;
    rmap["$13"] = 13;
    rmap["$t6"] = 14;
    rmap["$14"] = 14;
    rmap["$t7"] = 15;
    rmap["$15"] = 15;
    rmap["$s0"] = 16;
    rmap["$16"] = 16;
    rmap["$s1"] = 17;
    rmap["$17"] = 17;
    rmap["$s2"] = 18;
    rmap["$18"] = 18;
    rmap["$s3"] = 19;
    rmap["$19"] = 19;
    rmap["$s4"] = 20;
    rmap["$20"] = 20;
    rmap["$s5"] = 21;
    rmap["$21"] = 21;
    rmap["$s6"] = 22;
    rmap["$22"] = 22;
    rmap["$s7"] = 23;
    rmap["$23"] = 23;
    rmap["$t8"] = 24;
    rmap["$24"] = 24;
    rmap["$t9"] = 25;
    rmap["$25"] = 25;
    rmap["$k0"] = 26;
    rmap["$26"] = 26;
    rmap["$k1"] = 27;
    rmap["$27"] = 27;
    rmap["$gp"] = 28;
    rmap["$28"] = 28;
    rmap["$sp"] = 29;
    rmap["$29"] = 29;
    rmap["$fp"] = 30;
    rmap["$30"] = 30;
    rmap["$ra"] = 31;
    rmap["$31"] = 31;

    rrmap[0] = "$zero";
    rrmap[1] = "$at";
    rrmap[2] = "$v0";
    rrmap[3] = "$v1";
    rrmap[4] = "$a0";
    rrmap[5] = "$a1";
    rrmap[6] = "$a2";
    rrmap[7] = "$a3";
    rrmap[8] = "$t0";
    rrmap[9] = "$t1";
    rrmap[10] = "$t2";
    rrmap[11] = "$t3";
    rrmap[12] = "$t4";
    rrmap[13] = "$t5";
    rrmap[14] = "$t6";
    rrmap[15] = "$t7";
    rrmap[16] = "$s0";
    rrmap[17] = "$s1";
    rrmap[18] = "$s2";
    rrmap[19] = "$s3";
    rrmap[20] = "$s4";
    rrmap[21] = "$s5";
    rrmap[22] = "$s6";
    rrmap[23] = "$s7";
    rrmap[24] = "$t8";
    rrmap[25] = "$t9";
    rrmap[26] = "$k0";
    rrmap[27] = "$k1";
    rrmap[28] = "$gp";
    rrmap[29] = "$sp";
    rrmap[30] = "$fp";
    rrmap[31] = "$ra";
    return;
}



void CORE::RemoveSpaces()
{
    int32_t j = 0;
    while (j < currentInst.size() && (currentInst[j] == ' ' || currentInst[j] == '\t'))
        j++;
    currentInst = currentInst.substr(j); //remove all of those
    return;
}

void CORE::findRegister(int i)
{

    if (currentInst[0] != '$' || currentInst.size() < 2)
    {
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Register Not Found");
        return; // raise exception
    }

    string first5 = "", first3 = "", first2 = "";

    first2 = currentInst.substr(0, 2);
    if (currentInst.size() >= 3)
        first3 = currentInst.substr(0, 3);
    if (currentInst.size() >= 5)
        first5 = currentInst.substr(0, 5);

    if (first5 == "$zero")
    {
        CIRS[i] = 0;
        currentInst = currentInst.substr(5);
        return;
    }
    if (rmap.find(first3) != rmap.end())
    {
        CIRS[i] = rmap.at(first3);
        currentInst = currentInst.substr(3);
        return;
    }
    if (rmap.find(first2) != rmap.end())
    {
        CIRS[i] = rmap.at(first2);
        currentInst = currentInst.substr(2);
        return;
    }

    error.push_back("Synatx Error:" + to_string(currentInstNum) + ": Unknown Register");
    return;
}

void CORE::removeComma()
{
    if (currentInst.size() < 2 || currentInst[0] != ',')
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Comma Expected");
    currentInst = currentInst.substr(1);
    return;
}

void CORE::findImmediate()
{
    int i = 0;
    bool isValidInt = true;
    string buffer;
    if (currentInst[0] == '-')
    {
        buffer = "-";
        currentInst = currentInst.substr(1);
    }
    else
        i = 0;

    while (i < currentInst.size() && currentInst[i] < 58 && currentInst[i] > 47)
    {
        if (currentInst[i] == ' ' || currentInst[i] == '\t' || currentInst[i] == '(')
            break;
        buffer += currentInst[i];
        i++;
    }
    CIRS[2] = stoi(buffer);
    currentInst = currentInst.substr(i);
}

string CORE::findLabel()
{
    RemoveSpaces();
    string buffer = "";
    int i = 0;
    while (i < currentInst.size())
    {
        if (currentInst[i] == ' ' || currentInst[i] == '\t')
            break;
        buffer += currentInst[i];
        i++;
    }

    currentInst = currentInst.substr(i);
    RemoveSpaces();
    if (!currentInst.empty())
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Invalid Label Type");
    return buffer;
}



void CORE::offsetType()
{
    string buffer = "";
    string buffer1 = "";
    int i = 0;
    int offset = 0;
    int final1 = 0;
    bool bracketed = true;
    if (currentInst[0] == '-')
    {
        buffer = "-";
        currentInst = currentInst.substr(1);
    }
    else
        i = 0;

    while (i < currentInst.size() && currentInst[i] < 58 && currentInst[i] > 47)
    {
        if (currentInst[i] == ' ' || currentInst[i] == '\t' || currentInst[i] == '(')
            break;
        buffer += currentInst[i];
        i++;
    }
    currentInst = currentInst.substr(i);
    RemoveSpaces();
    int indexOfLparen = currentInst.find("(");
    if (indexOfLparen == -1)
        bracketed = false;
    else
        bracketed = true;

    int indexOfRparen = currentInst.find(")");
    if (bracketed && (indexOfRparen == -1 || indexOfRparen < indexOfLparen))
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unmatched Parenthesis");
    else if (bracketed)
    {

        string strInsideParen;
        if (bracketed)
            strInsideParen = currentInst.substr(indexOfLparen + 1, (indexOfRparen - indexOfLparen - 1));
        else
            strInsideParen = currentInst;

        RemoveSpaces();
        if (strInsideParen.size() > 0)
        {
            RemoveSpaces();
            int ValOfRegister = 0;
            int offset1 = 0;

            if (strInsideParen[0] == '$')
            {
                if (strInsideParen.size() < 2)
                {
                    error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Register Not Found");
                    return; // raise exception
                }

                string first5 = "", first3 = "", first2 = "";

                first2 = strInsideParen.substr(0, 2);
                if (strInsideParen.size() >= 3)
                    first3 = strInsideParen.substr(0, 3);
                if (strInsideParen.size() >= 5)
                    first5 = strInsideParen.substr(0, 5);

                if (first5 == "$zero")
                {
                    buffer1 = first5;
                    ValOfRegister = registers[0];
                    strInsideParen = strInsideParen.substr(5);
                }
                else if (rmap.find(first3) != rmap.end())
                {
                    buffer1 = first3;
                    ValOfRegister = registers[rmap.at(first3)];
                    strInsideParen = strInsideParen.substr(3);
                }
                else if (rmap.find(first2) != rmap.end())
                {
                    buffer1 = first2;
                    ValOfRegister = registers[rmap.at(first2)];
                    strInsideParen = strInsideParen.substr(2);
                }
                else
                {
                    error.push_back("Synatx Error:" + to_string(currentInstNum) + ": Register Not Found");
                    return;
                };
                final1 = ValOfRegister;
            }

            else
            { // string inside parenthesis does not start with $
                int i1 = 0;

                if (strInsideParen[0] == '-')
                {
                    buffer1 = "-";
                    strInsideParen = strInsideParen.substr(1);
                }
                else
                    i1 = 0;

                while (i1 < strInsideParen.size() && strInsideParen[i1] < 58 && strInsideParen[i1] > 47)
                {
                    if (strInsideParen[i1] == ' ' || strInsideParen[i1] == '\t' || strInsideParen[i1] == ')')
                        break;
                    buffer1 += strInsideParen[i1];
                    i1++;
                }
            }
        }

        if (rmap.find(buffer1) != rmap.end())
        {
            CIRS[3] = 0;
            CIRS[2] = rmap.at(buffer1);
        }
        else
        {
            CIRS[3] = 1;
            for (int i = 0; i < buffer1.size(); i++)
            {
                if ((buffer1[i] <= 47 || buffer1[i] >= 58) && buffer1[i] != '-')
                {
                    error.push_back("Synatx Error:" + to_string(currentInstNum) + ": Not a valid parameter Inside Parenthesis");
                    return;
                }
            }
            CIRS[2] = stoi(buffer1);
        }

        if (bracketed)
            currentInst = currentInst.substr(indexOfRparen + 1);
        else
            currentInst = currentInst.substr(0);
    }

    for (int i = 0; i < buffer.size(); i++)
    {
        if ((buffer[i] <= 47 || buffer[i] >= 58) && buffer[i] != '-')
        {
            error.push_back("Synatx Error:" + to_string(currentInstNum) + ": Not a Valid Offset");
            return;
        }
    }
    offset = stoi(buffer);
    offset += offsetOfCore;

    CIRS[1] = offset;
    return;
}

instruction CORE::readInst()
{
    int j = 0;
    string lableInst;
    vector<int> mynullv = {-1, -1, -1};
    instruction mynull(mynullv, -1);

    if (currentInst == "")
        return mynull;
    if (currentInst.size() < 4)
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Instruction Not Valid");
    for (j = 0; j < 4; j++)
        if (currentInst[j] == ' ' || currentInst[j] == '\t')
            break;

    string operation = currentInst.substr(0, j);
    currentInst = currentInst.substr(j + 1);

    bool isValidOp = false;
    int operationId = -1;
    for (int i = 0; i < myset.size(); i++)
    {
        if (myset[i] == operation)
        {
            isValidOp = true;
            operationId = i;
            break;
        }
    }

    if ((0 <= operationId) && (operationId < 4))
    { //add,sub,mul,slt
        for (int i = 0; i < 3; i++)
        {
            RemoveSpaces();
            findRegister(i);
            RemoveSpaces();
            if (i == 2)
                break;
            removeComma();
        }
        RemoveSpaces();
        if (currentInst != "")
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
    }

    else if (operationId == 4)
    { //addi
        for (int i = 0; i < 2; i++)
        {
            RemoveSpaces();
            findRegister(i);
            RemoveSpaces();
            if (i == 1)
                break;
            removeComma();
        }

        RemoveSpaces();
        removeComma();
        RemoveSpaces();
        findImmediate();
        RemoveSpaces();
        if (currentInst != "")
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
    }

    else if (operationId == 5 || operationId == 6)
    { //bne,beq
        for (int i = 0; i < 2; i++)
        {
            RemoveSpaces();
            findRegister(i);
            RemoveSpaces();
            if (i == 1)
                break;
            removeComma();
        }

        RemoveSpaces();
        removeComma();
        lableInst = findLabel();
        RemoveSpaces();
        if (currentInst != "")
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
    }

    else if (operationId == 7 || operationId == 8)
    { //lw,sw
        string tempstring = "";
        int offset;
        RemoveSpaces();
        findRegister(0);
        RemoveSpaces();
        removeComma();
        RemoveSpaces();
        if ((currentInst[0] > 47 && currentInst[0] < 58) || currentInst[0] == '-')
            offsetType();
        else
            error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unacceptable Label Type");
    }

    else if (operationId == 9)
    { //j label/address
        RemoveSpaces();
        lableInst = findLabel();
    }
    else if (operationId == -1)
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unknown Instruction");

    if (operationId == 9 || operationId == 5 || operationId == 6)
    {
        instruction inst(CIRS, operationId, lableInst);
        return inst;
    }
    else
    {
        instruction inst(CIRS, operationId);
        return inst;
    }
}

void CORE::preprocess()
{ // checking if line has label: at the beginning;
    int labelIndex;
    currentInstNum = 1;
    string tempLabel = "";
    for (int i = 0; i < numberOfLine; i++)
    {
        CIRS = {0, 0, 0, 0}; // re initializing CIRS to remove previous instructions data if any
        bool isLabel = false;
        currentInst = inputprogram[i];
        RemoveSpaces();
        if (currentInst != "")
        {
            RemoveSpaces();

            if (count(currentInst.begin(), currentInst.end(), ':') > 1)
                error.push_back("Syntax Error:" + to_string(currentInstNum) + ": \": Error: more than one label");
            labelIndex = currentInst.find(":");

            if (labelIndex == -1)
                isLabel = false;
            else if (labelIndex == 0)
                error.push_back("Syntax Error:" + to_string(currentInstNum) + ": \":\" At The Beginning Not Accepted");
            else
            {
                tempLabel = currentInst.substr(0, labelIndex);
                lableTable[tempLabel] = currentInstNum;
                currentInst = currentInst.substr(labelIndex);
                RemoveSpaces();
                currentInst = currentInst.substr(1);
                RemoveSpaces();
                if (currentInst == "")
                    continue;
            }
        }
        else
            continue;

        instruction instObj = readInst();
        iset->push_back(instObj);
        numberofInst++;
        currentInstNum++;
    }
}








////////////////////////////////////////////////////

void CORE::add(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] + registers[cirs[2]];
    
    return;
}
void CORE::sub(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] - registers[cirs[2]];
    return;
}
void CORE::mul(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] * registers[cirs[2]];
    return;
}
void CORE::addi(vector<int> &cirs)
{
    registers[cirs[0]] = registers[cirs[1]] + cirs[2];
    return;
}
void CORE::bne(vector<int> &cirs, string &label)
{
    if (lableTable.find(label) == lableTable.end())
    {
        runtimeError = "Error: Label Not Found";
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
            runtimeError = "Error: Label Not Found";
            return;
        }
    }
    else
        switchOnBranch = false;
    return;
}
void CORE::beq(vector<int> &cirs, string &label)
{
    if (lableTable.find(label) == lableTable.end())
    {
        runtimeError = "Error: Label Not Found";
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
            runtimeError = "Error: Label Not Found";
            return;
        }
    }
    else
        switchOnBranch = false;
    return;
}

void CORE::slt(vector<int> &cirs)
{
    if (registers[cirs[1]] < registers[cirs[2]])
        registers[cirs[0]] = 1;
    else
        registers[cirs[0]] = 0;
    return;
}



void CORE::j(vector<int> &cirs, string &label)
{
    pc = lableTable[label];
    return;
}