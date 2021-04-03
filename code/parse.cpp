#include "parse.hpp"

void RemoveSpaces()
{
    int32_t j = 0;
    while (j < currentInst.size() && (currentInst[j] == ' ' || currentInst[j] == '\t'))
        j++;
    currentInst = currentInst.substr(j); //remove all of those
    return;
}

void findRegister(int i)
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

void removeComma()
{
    if (currentInst.size() < 2 || currentInst[0] != ',')
        error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Comma Expected");
    currentInst = currentInst.substr(1);
    return;
}

void findImmediate()
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

string findLabel()
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

void offsetType()
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

    CIRS[1] = offset;
    return;
}

instruction readInst()
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

void preprocess()
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
        iset.push_back(instObj);
        numberofInst++;
        currentInstNum++;
    }
    return;
}