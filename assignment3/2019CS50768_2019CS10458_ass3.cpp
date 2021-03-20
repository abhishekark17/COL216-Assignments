#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <unordered_set>
using namespace std;
//Max int val: 	2147483647

vector<int> CIRS (4,0);   //current instruction register set

/*
CIRS DEFINITION FOR VARIOUS INSTRUCTIONS:
add,sub,mul, slt := [register1 ,register2 ,register3 , NotUsed]
addi := [register1,register2,immediateValue,NotUsed]

lw, sw := [register1, offset , last register , 0] // 0 if register is there as last arguement inside parenthesis
       := [register1, offset ,address , 1]  // 1 if address given as argument inside braces
bne, beq := [register1, register2, NotUsed, NotUsed]
j  := [NotUsed, NotUsed, NotUsed, NotUsed]
*/

class instruction {
  public:
  vector<int> cirs;
  int opID;
  string label="";
  instruction (vector<int> CIRS , int id) {
    cirs = CIRS;
    opID = id;
  }
  instruction (vector<int> CIRS , int id,string str) {
    cirs = CIRS;
    opID = id;
    label=str;
  }
  instruction () {
    cirs = {-1,-1,-1,-1};
    opID = -1;
  }
};

int sizeOfData;
string error1 = "";
vector<string> error;
// int maxAllowedInst=100; int startMemOfData=400;

vector<int> data123 (1<<18,0);    // all memory addresses
vector<int> numOfInst (10,0);   // which instruction is called how many times
int numberofInst=0;


int numClockCycles = 0;
unordered_map <string,int> rmap;
vector<string> inputprogram;
vector<instruction> iset;

vector<int> registers(32,0);
string file , outfile;

unordered_map<string,int> lableTable; //which lable is present on which instruction number
unordered_map <int,string> uniqueLabelID;

int numberOfLine = 0 , currentInstNum = 0, maxInstructions = 0 ;
string currentInst;
int labelNumber = 0;

vector<string> myset = {"add","sub","mul","slt","addi","bne","beq","lw","sw","j"};
//                        0     1     2     3      4     5     6     7    8   9       opIDs    //
//                        R     R     R     R     I      I     I    

int pc = 1;   // program counter  //

void printVector(ofstream& out) {
  cout << "[";
  out << "[";
  for(int i=0;i<31;i++){
    if(registers[rmap["$zero"]]!=0){
      error1 = "Error: Register Zero Cannot Be Modified";
      return;
    }

    string s="$";
    s=s+to_string(i);
    cout<< s << ": " << hex << registers[rmap[s]] << dec << ",";
    out << s << ": " << hex << registers[rmap[s]] << dec << ",";
  }

  string s="$";
  s=s+to_string(31);
  cout<<s<<": "<<hex<<registers[rmap[s]]<<dec;
  out<<s<<": "<<registers[rmap[s]]<<dec;
  cout  << "]" << endl;
  out  << "]" << endl;
}

void initialize_registers () {
  rmap["$zero"] = 0; rmap["$0"] = 0;
  rmap["$at"] = 1; rmap["$1"] = 1;
  rmap["$v0"] = 2; rmap["$2"] = 2;
  rmap["$v1"] = 3; rmap["$3"] = 3;
  rmap["$a0"] = 4; rmap["$4"] = 4;
  rmap["$a1"] = 5; rmap["$5"] = 5;
  rmap["$a2"] = 6; rmap["$6"] = 6;
  rmap["$a3"] = 7; rmap["$7"] = 7;
  rmap["$t0"] = 8; rmap["$8"] = 8;
  rmap["$t1"] = 9; rmap["$9"] = 9;
  rmap["$t2"] = 10; rmap["$10"] = 10;
  rmap["$t3"] = 11; rmap["$11"] = 11;
  rmap["$t4"] = 12; rmap["$12"] = 12;
  rmap["$t5"] = 13; rmap["$13"] = 13;
  rmap["$t6"] = 14; rmap["$14"] = 14;
  rmap["$t7"] = 15; rmap["$15"] = 15;
  rmap["$s0"] = 16; rmap["$16"] = 16;
  rmap["$s1"] = 17; rmap["$17"] = 17;
  rmap["$s2"] = 18; rmap["$18"] = 18;
  rmap["$s3"] = 19; rmap["$19"] = 19;
  rmap["$s4"] = 20; rmap["$20"] = 20;
  rmap["$s5"] = 21; rmap["$21"] = 21;
  rmap["$s6"] = 22; rmap["$22"] = 22;
  rmap["$s7"] = 23; rmap["$23"] = 23;
  rmap["$t8"] = 24; rmap["$24"] = 24;
  rmap["$t9"] = 25; rmap["$25"] = 25;
  rmap["$k0"] = 26; rmap["$26"] = 26;
  rmap["$k1"] = 27; rmap["$27"] = 27;
  rmap["$gp"] = 28; rmap["$28"] = 28;
  rmap["$sp"] = 29; rmap["$29"] = 29;
  rmap["$fp"] = 30; rmap["$30"] = 30;
  rmap["$ra"] = 31; rmap["$31"] = 31;  
  return;
}


void readFile(string file){
  ifstream ifs (file);
  string myline;
  while (ifs.good()) {
    getline (ifs,myline);
    numberOfLine++;
    inputprogram.push_back(myline);
  }
  ifs.close();
}

void RemoveSpaces(){
	int32_t j=0;
	while(j<currentInst.size() && (currentInst[j]==' ' || currentInst[j]=='\t')) j++;
	currentInst=currentInst.substr(j); //remove all of those
}

void findRegister (int i) {

  if (currentInst[0] != '$' || currentInst.size() < 2) {
    error.push_back("Syntax Error:"+to_string(currentInstNum)+": Register Not Found");
    return; // raise exception 
  }

  string first5 = "", first3 = "", first2 = "";
  
  first2 = currentInst.substr(0,2);
  if (currentInst.size()>= 3) first3 = currentInst.substr(0,3);
  if (currentInst.size() >= 5) first5 = currentInst.substr(0,5);

  if (first5 == "$zero") {
    CIRS[i] = 0;
    currentInst = currentInst.substr(5);
    return;
  }
  if (rmap.find(first3) != rmap.end()) {
    CIRS[i] = rmap.at(first3);
    currentInst = currentInst.substr(3);
    return;
  }
  if (rmap.find(first2) != rmap.end()) {
    CIRS[i] = rmap.at(first2);
    currentInst = currentInst.substr(2);
    return;
  }
  
  error.push_back("Synatx Error:"+to_string(currentInstNum)+": Unknown Register");
  return;
}

void removeComma() {
  if (currentInst.size() < 2 || currentInst[0] != ',') error.push_back("Syntax Error:"+to_string(currentInstNum)+": Comma Expected");
  currentInst = currentInst.substr(1);
  return;
}

void findImmediate() {
  int i=0;
  bool isValidInt=true;
  string buffer = "";
  if (currentInst[0] == '-') {
    buffer += "-"; 
    currentInst = currentInst.substr(1);
    RemoveSpaces();
  }
  for(i=0;i<currentInst.size();i++){
    if(currentInst[i]==' ' || currentInst[i]=='\t') break;
    else if(currentInst[i]>47 && currentInst[i]<58) buffer+=currentInst[i];
    else {
      isValidInt=false;
      break;
    }
  }

  if(isValidInt) CIRS[2]=stoi(buffer);
  currentInst=currentInst.substr(i);
}


string findLabel(){
  RemoveSpaces();
  string buffer = "";
  int i = 0;
  while (i < currentInst.size()) {
    if (currentInst[i] == ' ' || currentInst[i] == '\t') break;
    buffer += currentInst[i];
    i++;  
  }

  currentInst = currentInst.substr(i);
  RemoveSpaces();
  if (!currentInst.empty()) error.push_back("Syntax Error:"+to_string(currentInstNum)+": Invalid Label Type");
  return buffer;
}

void offsetType() {
  string buffer = "";
  string buffer1 = "";
  int i = 1;
  int offset = 0;
  int final1 = 0;
  bool bracketed = true;
  if(currentInst[0] == '-') {
    buffer = "-"; 
    currentInst = currentInst.substr(1);
  }
  else i = 0;

  while (i < currentInst.size() && currentInst[i] < 58 && currentInst[i] > 47 ) {
    if (currentInst[i] == ' ' || currentInst[i] == '\t' || currentInst[i]=='(') break;
    buffer += currentInst[i];
    i++;
  }
  currentInst=currentInst.substr(i);

  int indexOfLparen=currentInst.find("(");
  if(indexOfLparen==-1) bracketed=false;
  else bracketed=true;

  int indexOfRparen=currentInst.find(")");
  if(bracketed && (indexOfRparen==-1 || indexOfRparen<indexOfLparen)) error.push_back("Syntax Error:"+to_string(currentInstNum)+": Unmatched Parenthesis");
  else if (bracketed){

    string strInsideParen;
    if(bracketed) strInsideParen = currentInst.substr(indexOfLparen+1,(indexOfRparen-indexOfLparen-1));
    else strInsideParen=currentInst;
  
    RemoveSpaces();
    if(strInsideParen.size()>0) {
      RemoveSpaces();
      int ValOfRegister = 0;
      int offset1= 0;

      if (strInsideParen[0] == '$') {
        if (strInsideParen.size() < 2) {
          error.push_back("Syntax Error:"+to_string(currentInstNum)+": Register Not Found");
          return; // raise exception 
        } 

        string first5 = "", first3 = "", first2 = "";

        first2 = strInsideParen.substr(0,2);
        if (strInsideParen.size()>= 3) first3 = strInsideParen.substr(0,3);
        if (strInsideParen.size() >= 5) first5 = strInsideParen.substr(0,5);
      
        if (first5 == "$zero") {
          buffer1=first5;
          ValOfRegister = registers[0];
          strInsideParen = strInsideParen.substr(5);
        }
        else if (rmap.find(first3) != rmap.end()) {
          buffer1=first3;
          ValOfRegister = registers[rmap.at(first3)];
          strInsideParen = strInsideParen.substr(3);
        }
        else if (rmap.find(first2) != rmap.end()) {
          buffer1=first2;
          ValOfRegister = registers[rmap.at(first2)];
          strInsideParen = strInsideParen.substr(2);
        }
        else {error.push_back("Synatx Error:"+to_string(currentInstNum)+": Register Not Found"); return;};
        final1 = ValOfRegister;
      }

      else {  // string inside parenthesis does not start with $
        int i1 = 1;
    
        if(strInsideParen[0]=='-') {
          buffer1="-"; 
          strInsideParen = strInsideParen.substr(1);
        }
        else i1=0;

        while (i1 < strInsideParen.size() && strInsideParen[i1] < 58 && strInsideParen[i1] > 47) {
          if (strInsideParen[i1] == ' ' || strInsideParen[i1] == '\t' || strInsideParen[i1] == ')') break;
          buffer1 += strInsideParen[i1];
          i1++;
        }    
      }
    }

    if(rmap.find(buffer1)!=rmap.end()){
      CIRS[3]=0;
      CIRS[2]=rmap.at(buffer1);
    }
    else{
      CIRS[3]=1;
      for(int i=0;i<buffer1.size();i++){
        if ((buffer1[i]<=47 || buffer1[i]>=58) && buffer1[i]!='-' ){
          error.push_back("Syntax Error:"+to_string(currentInstNum)+": Not a valid parameter Inside Parenthesis");
          return;
        }  
      }
      CIRS[2]=stoi(buffer1);
    }

    if(bracketed) currentInst=currentInst.substr(indexOfRparen+1);
    else currentInst=currentInst.substr(0);
  }
  
  for(int i=0;i<buffer.size();i++){
    if ((buffer[i]<=47 || buffer[i]>=58) && buffer[i]!='-' ) {
      error.push_back("Syntax Error:"+to_string(currentInstNum)+": Not a Valid Offset");
      return;
    }
  }
  offset=stoi(buffer);

  CIRS[1] = offset;
  return;
}

instruction readInst () {
  int j = 0;
  string lableInst;
  vector<int> mynullv = {-1,-1,-1};
  instruction mynull(mynullv,-1);

  if(currentInst == "") return mynull;
  if (currentInst.size() < 4) error.push_back("Syntax Error:"+to_string(currentInstNum)+": Instruction Not Valid");
  for(j = 0; j < 4; j++) if(currentInst[j] == ' ' || currentInst[j] == '\t') break;

  string operation = currentInst.substr(0,j);
  currentInst = currentInst.substr(j+1);

  bool isValidOp = false;
  int operationId = -1;
  for (int i = 0; i < myset.size(); i++) {
    if (myset[i] == operation) {
      isValidOp = true;
      operationId = i;
      break;
    }
  }
  
  if((0 <= operationId) && (operationId < 4)) { //add,sub,mul,slt
    for(int i = 0; i < 3; i++){
      RemoveSpaces();
      findRegister(i);
      RemoveSpaces();
      if(i == 2) break;
      removeComma();
    }
    RemoveSpaces();
    if(currentInst != "") error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
  }

  else if(operationId == 4){//addi
    for(int i = 0; i < 2; i++){
      RemoveSpaces();
      findRegister(i);
      RemoveSpaces();
      if(i == 1) break;
      removeComma();
    }

    RemoveSpaces(); removeComma(); RemoveSpaces();
    findImmediate();
    RemoveSpaces();
    if(currentInst != "") error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Number Of Arguments Exceeded");
  }

  else if(operationId == 5 || operationId == 6){//bne,beq
    for(int i = 0; i < 2; i++){
      RemoveSpaces();
      findRegister(i);
      RemoveSpaces();
      if(i == 1) break;
      removeComma();
    }

    RemoveSpaces(); removeComma();
    lableInst=findLabel();
    RemoveSpaces();
    if(currentInst!="") error.push_back("Syntax Error:"+to_string(currentInstNum)+": Number Of Arguments Exceeded");
  }

  else if(operationId==7 || operationId==8){//lw,sw
    string tempstring="";
    int offset;
    RemoveSpaces();
    findRegister(0);
    RemoveSpaces(); removeComma(); RemoveSpaces();
    if((currentInst[0]>47 && currentInst[0]<58) || currentInst[0]=='-' ) offsetType();
    else error.push_back("Syntax Error:"+to_string(currentInstNum)+": Unacceptable Label Type");
  }

  else if (operationId==9){//j label/address
    RemoveSpaces();
    lableInst=findLabel();
  }
  else if (operationId == -1) error.push_back("Syntax Error:" + to_string(currentInstNum) + ": Unknown Instruction");

  if( operationId == 9 || operationId == 5 || operationId == 6 ) {
    instruction inst(CIRS,operationId,lableInst);
    return inst;
  }
  else{
    instruction inst(CIRS,operationId);
    return inst;
  }
}


void preprocess () {  // checking if line has label: at the beginning;
  int labelIndex;
  currentInstNum = 1;
  string tempLabel = "";
  for (int i = 0; i < numberOfLine ; i++) {
    CIRS = {0,0,0,0};   // re initializing CIRS to remove previous instructions data if any
    bool isLabel = false;
    currentInst = inputprogram[i];
    RemoveSpaces();
    if (currentInst != "") {
      RemoveSpaces();

      if (count(currentInst.begin(), currentInst.end(), ':') > 1) error.push_back("Syntax Error:"+to_string(currentInstNum)+": \": Error: more than one label");
      labelIndex=currentInst.find(":");

      if (labelIndex== -1) isLabel = false;
      else if (labelIndex==0)   error.push_back("Syntax Error:"+to_string(currentInstNum)+": \":\" At The Beginning Not Accepted");
      else {
        tempLabel = currentInst.substr(0,labelIndex);
        lableTable[tempLabel]=currentInstNum;
        currentInst = currentInst.substr(labelIndex);
        RemoveSpaces();
        currentInst=currentInst.substr(1);
        RemoveSpaces();
        if(currentInst=="") continue;
      }  
    }
    else continue;

    instruction instObj = readInst();
    iset.push_back(instObj);
    numberofInst++;
    currentInstNum++;
  }
  return;
}


void add (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] + registers[cirs[2]];
  return;
}
void sub (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] - registers[cirs[2]];
  return;
}
void mul (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] * registers[cirs[2]];
  return;
}
void addi (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] + cirs[2];
  return;
}
void bne (vector<int>& cirs,string& label) {
  if(lableTable.find(label)==lableTable.end()){
     error1="Error: Label Not Found";
      return;
  }
  if (registers[cirs[0]] != registers[cirs[1]]) {
    if(lableTable.find(label)!=lableTable.end()) pc = lableTable[label];
    else{
      error1="Error: Label Not Found";
      return;
    }
  }

  return;
}
void beq (vector<int>& cirs,string& label) {
    if(lableTable.find(label)==lableTable.end()){
     error1="Error: Label Not Found";
      return;
  }

  if (registers[cirs[0]] == registers[cirs[1]]) {
    if(lableTable.find(label)!=lableTable.end()) pc = lableTable[label];
    else{
      error1="Error: Label Not Found";
      return;
    }
  }
  return;
}

void slt (vector<int>& cirs) {
  if (registers[cirs[1]] < registers[cirs[2]]) registers[cirs[0]] = 1;
  else registers[cirs[0]] = 0;
  return;
}

void lw (vector<int>& cirs) {
  int offset=cirs[1];
  int typeOfarg=cirs[3];
  int address;

  if(typeOfarg==0) address=offset+registers[cirs[2]];
  else address=offset+cirs[2];
  if(address<4*numberofInst || address>1048576) {
    error1="Error: Memory Address not accessible";
    return;
    }
  else if(address%4 != 0) {
    error1 = "Error: invalid memory location";
    return;
    }
  else {
    address /= 4; 
    address -= numberofInst;
  }
  registers[cirs[0]] = data123[address];
  return;
}

void sw (vector<int>& cirs) {
  int offset=cirs[1];
  int typeOfarg=cirs[3];
  int address;
  if(typeOfarg==0) address=offset+registers[cirs[2]];
  else address=offset+cirs[2];
  
  if(address<4*numberofInst || address>1048576) {
    error1="Error: Memory Address not accessible";
    return;
  }
  else if(address%4 != 0) {
    error1 = "Error: invalid memory location";
    return;
  }
  else { 
    address /= 4;
    address -= numberofInst; 
  }
  data123[address] = registers[cirs[0]];
  return;
}

void j (vector<int>& cirs,string& label) {
  pc = lableTable[label];
  return;
}

void execute (ofstream& out) {
  while (pc <= maxInstructions) {
   
    if(error1 != "") {
      cout << "Runtime Error:" << pc-1 << ":" << error1 << endl;
      return;
    }
    numClockCycles++;
    instruction currInst = iset[pc-1];
    pc++;

    switch (currInst.opID) {
      case 0: { //add 
        add (currInst.cirs);
        numOfInst[0]++;
        break;
      }
      case 1: { //sub
        sub (currInst.cirs);
        numOfInst[1]++;
        break;
      }
      case 2: { //mul
        mul (currInst.cirs);
        numOfInst[2]++;
        break;
      }
      case 3: { // slt
        slt (currInst.cirs);
        numOfInst[3]++;
        break;
      }
      case 4: { //addi
        addi(currInst.cirs);
        numOfInst[4]++;
        break;
      }
      case 5: { //bne
        bne (currInst.cirs,currInst.label);
        numOfInst[5]++;
        break;
      }
      case 6: { //beq
        beq(currInst.cirs,currInst.label);
        numOfInst[6]++;
        break;
      }
      case 7: { //lw
        lw(currInst.cirs);
        numOfInst[7]++;
        break;
      }
      case 8: { //sw
        sw(currInst.cirs);
        numOfInst[8]++;
        break;
      }
      case 9: { //j
        j (currInst.cirs,currInst.label);
        numOfInst[9]++;
        break;
      }
    };
    cout<<"Cycle Number: "<<numClockCycles<<endl;
    out<<"Cycle Number: "<<numClockCycles<<endl;
    printVector(out);
  }

  if(error1 != "") {
      cout << "Runtime Error:" << pc << ":" << error1 <<endl;
      return;
  }
  return;
}


int main(int argc, char** argv) {
  initialize_registers();
  registers[rmap["$sp"]]=(1<<20)-4;
  if (argc==1){
		cout<<"Input File should be given as argument"<<endl;
    return 0;	
	}
	else{
		file=argv[1];
    if(argc==3) outfile=argv[2];
    else outfile="output.txt";
	}
  
  readFile(file);
  preprocess(); // all input has been read and stored in iset.
  sizeOfData=(1<<18) - numberofInst;
  
  for(int i=250-numberofInst;i<250-numberofInst+10;i++) data123[i] = i-(250-numberofInst);
  
  maxInstructions=numberofInst;
  ofstream outstream(outfile);
  if(error.size()>0){
    for(int i=0;i<error.size();i++){
      cout<<error[i]<<"\n"<<endl;
      outstream<<error[i]<<"\n"<<endl;  
    }
    outstream.close();
    return 0;
  }

  execute (outstream);

  cout<<endl; outstream<<endl;
  cout<<"Total Number of Cycles: "<<numClockCycles<<endl<<endl;
  outstream<<"Total Number of Cycles: "<<numClockCycles<<endl<<endl;
  cout<<"["; outstream<<"[";

  int totalValidinstruction=0;
  for(int i=0;i<numOfInst.size()-1;i++){
    totalValidinstruction+=numOfInst[i];
    cout<<myset[i]<<": "<<numOfInst[i]<<",";
    outstream<<myset[i]<<": "<<numOfInst[i]<<",";
  }
  cout<<myset[numOfInst.size()-1]<<": "<<numOfInst[numOfInst.size()-1]<<"]"<<endl;
  outstream<<myset[numOfInst.size()-1]<<": "<<numOfInst[numOfInst.size()-1]<<"]"<<endl;
  cout<<endl; outstream<<endl;
  cout<<"Final data values that are updated during execution:"<<endl;
  outstream<<"Final data values that are updated during execution:"<<endl;

  for(int i=250-numberofInst;i<250-numberofInst+10;i++){
    cout<<(i+numberofInst)*4<<"-"<<(i+numberofInst)*4+3<<": "<<hex<<data123[i]<<dec<<endl;
    outstream<<(i+numberofInst)*4<<"-"<<(i+numberofInst)*4+3<<": "<<hex<<data123[i]<<dec<<endl;
  } 
  //cout<<pc<<endl;
  //cout<<numberofInst<<endl;
  //totalValidinstruction+=numOfInst[numOfInst.size()-1];
  //cout<<myset[numOfInst.size()-1]<<": "<<numOfInst[numOfInst.size()-1]<<","<<"NumberOfEmptyInstruction: "<<(numberOfLine-totalValidinstruction) <<"]"<<endl;
  //outstream<<myset[numOfInst.size()-1]<<": "<<numOfInst[numOfInst.size()-1]<<","<<"NumberOfEmptyInstruction: "<<(numberOfLine-totalValidinstruction) <<"]"<<endl;
  //cout<<numberOfLine<<","<<sizeOfData<<endl;
  outstream.close();
  return 0;
}