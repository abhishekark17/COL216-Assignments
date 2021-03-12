#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <unordered_set>
using namespace std;
//Max int val: 	2147483647


vector<int> CIRS (3,0);   //current instruction register set
class instruction {
  public:
  vector<int> cirs;
  int opID;
  instruction (vector<int> CIRS , int id) {
    cirs = CIRS;
    opID = id;
  }
  instruction () {
    cirs = {-1,-1,-1};
    opID = -1;
  }
};

int maxAllowedInst=100;
int startMemOfData=400;

vector<int> data123 (262044,0);

string error="";
int numClockCycles = 0;
unordered_map <string,int> rmap;
vector<string> inputprogram;
vector<instruction> iset;


vector<int> registers(32,0);
string file = "test.txt";

unordered_map<string,int> lableTable;
unordered_map <int,string> uniqueLabelID;

int numberofInst = 0;
string currentInst;
int currentInstNum = 0;
int maxInstructions = 0;

int labelNumber = 0;



vector<string> myset = {"add","sub","mul","slt","addi","bne","beq","lw","sw","j"};

int pc = 0;

// sw $5, 8($7) # mem[$7+8] <- $5
//$7 is the register holding the memory address, 8 an offset and $5 is the source of the information that will be written in memory.

void printVector() {
  cout << "["<<endl;
  for(int i=0;i<32;i++){
    string s="$";
    s=s+to_string(i);
    cout<<s<<": "<<registers[rmap[s]]<<endl;
  }
  cout  << "]" << endl;
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
    numberofInst++;
    inputprogram.push_back(myline);
  }
  ifs.close();

}

void RemoveSpaces(){
	int32_t j=0;
	while(j<currentInst.size() && (currentInst[j]==' ' || currentInst[j]=='\t')) j++;
	currentInst=currentInst.substr(j); //remove all of those
}








// bne beq addi -- I type


void findRegister (int i) {

  int foundR = 0;
  if (currentInst[0] != '$' || currentInst.size() < 2) {
    cout << "Register Not Found" << endl;
    return; // raise exception 
  }

  //currentInst = currentInst.substr(1);
  //string givenR = currentInst.substr(0,2);
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
  
  cout << "Unknown Register";
  return;
  
}



void removeComma() {
  if (currentInst.size() < 2 || currentInst[0] != ',') {
    cout << "Error: Comma Expected" << endl;

  }
  currentInst = currentInst.substr(1);
  return;
}

void findImmediate() {

 int i=0;
 bool isValidInt=true;
 string buffer;
 //cout<<currentInst<<"fsadfas"<<endl;
 for(i=0;i<currentInst.size();i++){
     if(currentInst[i]==' ' || currentInst[i]=='\t'){
         break;
     }
     else if(currentInst[i]>47 && currentInst[i]<58){
         buffer+=currentInst[i];
     }
     else{
         isValidInt=false;
         cout<<"Not a valid integer"<<endl;
         break;
     }
 }
 //cout<<buffer<<"vsd"<<endl;
 if(isValidInt){
     CIRS[2]=stoi(buffer);
     cout<<"value"<<CIRS[2]<<endl;
 }
 
 currentInst=currentInst.substr(i);


 }




void findLabel(){
  string buffer = "";
  int i = 0;
  while (i < currentInst.size()) {
    if (currentInst[i] == ' ' || currentInst[i] == '\t') break;
    buffer += currentInst[i];
    i++;  
  }

  currentInst = currentInst.substr(i);
  if (!currentInst.empty()) cout << "Error: Invalid lable type" << endl;
  else{
    uniqueLabelID[labelNumber] = buffer;
    CIRS[2]=labelNumber;
    labelNumber++;
  }
  return;
  
}



void offsetType() {
  string buffer = "";
  int i = 1;
  int offset;
  if(currentInst[0]=='-') {
    buffer="-"; 
    currentInst = currentInst.substr(1);}
  else i=0;
  //cout<<"sadfj"<<endl;
  while (i < currentInst.size() && currentInst[i] < 58 && currentInst[i] > 47) {
    if (currentInst[i] == ' ' || currentInst[i] == '\t' || currentInst[i] == '(') break;
    buffer += currentInst[i];
    i++;
  }
  //cout<<"orut"<<endl;
  if(currentInst.size()>=2){
    offset=stoi(buffer);
  }

  else cout<<"Not a valid offset";
  //cout<<"offset"<<offset<<endl;
  currentInst = currentInst.substr(i);
  int final1=0;
  RemoveSpaces();
  if (currentInst[0] != '(' ) cout << "'(' EXPECTED" << endl;
  else {
    currentInst = currentInst.substr(1);
    RemoveSpaces();
    int ValOfRegister = 0;
    int offset1= 0;
    //
  if (currentInst[0] == '$') {
    if (currentInst[0] != '$' || currentInst.size() < 2) {
      cout << "Register Not Found" << endl;
      return; // raise exception 
    } 

    string first5 = "", first3 = "", first2 = "";

    first2 = currentInst.substr(0,2);
    if (currentInst.size()>= 3) first3 = currentInst.substr(0,3);
    if (currentInst.size() >= 5) first5 = currentInst.substr(0,5);

  
    if (first5 == "$zero") {
      ValOfRegister = registers[0];
      currentInst = currentInst.substr(5);
    }
    else if (rmap.find(first3) != rmap.end()) {
      ValOfRegister = registers[rmap.at(first3)];
      currentInst = currentInst.substr(3);
    }
    else if (rmap.find(first2) != rmap.end()) {
      ValOfRegister = registers[rmap.at(first2)];
      currentInst = currentInst.substr(2);
    }
    else {cout << "Unknown Register"; return;};
    final1 = ValOfRegister;
  }
  else {
    string buffer1 = "";
    int i1 = 1;
  
    if(currentInst[0]=='-') {
      buffer1="-"; 
      currentInst = currentInst.substr(1);}
    else i1=0;
    while (i1 < currentInst.size() && currentInst[i1] < 58 && currentInst[i1] > 47) {
      if (currentInst[i1] == ' ' || currentInst[i1] == '\t' || currentInst[i1] == ')') break;
      buffer1 += currentInst[i1];
      i1++;
    }
    //cout<<"final"<<endl;
    if(currentInst.size()>=2){
      offset1 = stoi(buffer1);
    }
    final1 = offset1;
    //cout<<final1<<"fdg"<<endl;

  }

  
}
CIRS[1] = final1 + offset;
cout<<CIRS[1]<<endl;
return;
}



instruction readInst () {
  int j=0;
  //instruction inst;
  vector<int> mynullv = {-1,-1,-1};
  instruction mynull(mynullv,-1);
  if(currentInst=="") return mynull;
  if (currentInst.size() < 4) error="Not a valid instruction";
  for(j=0;j<4;j++){
    if(currentInst[j]== ' ' || currentInst[j]== '\t'){
      break;
    }
  }
  string operation=currentInst.substr(0,j);
  currentInst=currentInst.substr(j+1);
    cout<<operation<<endl;
  bool isValidOp=false;
  int operationId=-1;
  for (int i=0;i<myset.size();i++) {
    if (myset[i]==operation) {
      isValidOp=true;
      operationId=i;
      break;
      }
    
  }
  

  if(operationId<4){ //add,sub,mul,slt
    for(int i=0;i<3;i++){
      RemoveSpaces();
      findRegister(i);
      RemoveSpaces();
      if(i==2) break;
      removeComma();
      
    }
    if(currentInst!="") {
      error="More than expected argument";
    }// raise exception
    
  }
  if(operationId==4){//addi
    for(int i=0;i<2;i++){
      RemoveSpaces();
      findRegister(i);
      RemoveSpaces();
      if(i==1) break;
      removeComma();
      
    }
    RemoveSpaces();
    removeComma();
    findImmediate();
    if(currentInst!="") {
      error="More than expected argument";
    }// raise exception
  }

  if(operationId==5 || operationId==6){//bne,beq
    for(int i=0;i<2;i++){
      RemoveSpaces();
      findRegister(i);
      RemoveSpaces();
      if(i==1) break;
      removeComma();
      
    }
    RemoveSpaces();
    findLabel();
    if(currentInst!="") {
      error="More than expected argument";
    }// raise exception
  }

  if(operationId==7 || operationId==8){//lw,sw
    string tempstring="";
    int offset;
    RemoveSpaces();
    findRegister(0);
    RemoveSpaces();
    removeComma();
    RemoveSpaces();
    if((currentInst[0]>47 && currentInst[0]<58) || currentInst[0]=='-' ){
      offsetType();
    }
    else cout<<"Label type not accepted";
    
  
  }

  if (operationId==9){//j label/address
    RemoveSpaces();
    findLabel();
  }
    cout<<operationId<<endl;
  instruction inst(CIRS,operationId);
  return inst;
}


//label:

//label:add $
//BRANCH1: add returnReg, reg1, $zero 





void preprocess () {  // checking if line has label: at the beginning;
  int labelIndex;
  currentInstNum=1;
  string tempLabel="";
  for (int i=0; i < numberofInst ; i++) {
    bool isLabel = false;
    currentInst = inputprogram[i];
    
    if (currentInst != ""){
        RemoveSpaces();

        if (count(currentInst.begin(), currentInst.end(), ':') > 1) 
        cout << "Error: more than one label" << endl;
        
        labelIndex=currentInst.find(":");

        if (labelIndex== -1) isLabel = false;
        else if (labelIndex==0)   cout << "Error: : in the beginning" << endl; // raise exception
        else {
        tempLabel = currentInst.substr(0,labelIndex);
        lableTable[tempLabel]=currentInstNum;
        currentInst = currentInst.substr(labelIndex);
        RemoveSpaces();
        currentInst=currentInst.substr(1);
        RemoveSpaces();
        

        }

    } 
    instruction instObj = readInst();
    iset.push_back(instObj);
    currentInstNum++;
  }

}
void add (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] + registers[cirs[2]];
  //pc++;
  return;
}
void sub (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] - registers[cirs[2]];
  //pc++;
  return;
}
void mul (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] * registers[cirs[2]];
  //pc++;
  return;
}

void addi (vector<int>& cirs) {
  registers[cirs[0]] = registers[cirs[1]] + cirs[2];
  cout<<"value:"<<cirs[2]<<endl;
  //pc++;
  return;
}

void bne (vector<int>& cirs) {
  if (registers[cirs[0]] != registers[cirs[1]]) {
      cout<<"label:"<<cirs[2]<<",";
      cout<<uniqueLabelID[cirs[2]]<<",";
      cout<<lableTable[uniqueLabelID[cirs[2]]]<<endl;;
    pc = lableTable[uniqueLabelID[cirs[2]]];
  }
  cout<<"label Id"<<pc<<endl;
  
  return;
}
void beq (vector<int>& cirs) {
  if (registers[cirs[0]] == registers[cirs[1]]) {
    pc = lableTable[uniqueLabelID[cirs[2]]];
  }
  return;
}

void slt (vector<int>& cirs) {
  if (registers[cirs[1]] < registers[cirs[2]]) {
    registers[cirs[0]] = 1;
  }
  else registers[cirs[0]] = 0;
  //pc++;
  return;
}
void lw (vector<int>& cirs) {
  int address=cirs[1];
  if(address<400 || address>1048576) error="Error: Memory Address not accessible\n";
  else if(address%4 != 0) error = "Error: invalid memory location\n";
  else {
    address /= 4; 
    address -= 100;
  }
  registers[cirs[0]] = data123[address];
    cout<<address<<","<<cirs[0]<<endl;
  //pc++;
  return;
}
void sw (vector<int>& cirs) {
  int address=cirs[1];
  if(address<400 || address>1048576) error="Error: Memory Address not accessible\n";
  else if(address%4 != 0) error = "Error: invalid memory location\n";
  else {
   
    address /= 4;
    address -= 100; 
  }
  data123[address] = registers[cirs[0]];

  //pc++;
  return;
}

void j (vector<int>& cirs) {
  pc = lableTable[uniqueLabelID[cirs[0]]];
  return;
}

void execute () {
  
  while (pc < maxInstructions) {
      cout<<"PC:"<<pc<<endl;

    instruction currInst = iset[pc];
    pc++;
//{"add","sub","mul","slt","addi","bne","beq","lw","sw","j"};
    switch (currInst.opID) {
      case 0: {
        //add 
        add (currInst.cirs);
        break;
      }
      case 1: {
        //sub
        sub (currInst.cirs);
        break;
      }
      case 2: {
        //mul
        sub (currInst.cirs);
        break;
      }
      case 3: {
        // slt
        slt (currInst.cirs);
        break;
      }
      case 4: {
        //addi
        addi(currInst.cirs);
        break;
      }
      case 5: {
        //bne
        bne (currInst.cirs);
        break;
      }
      case 6: {
        //beq
        beq(currInst.cirs);
        break;
      }
      case 7: {
        //lw
        lw(currInst.cirs);
        break;
      }
      case 8: {
        //sw
        sw(currInst.cirs);
        break;
      }
      case 9: {
        //j
        j (currInst.cirs);
        break;
      }
    };
  printVector();


  }



  return;
}


 int main() {
  initialize_registers();
  //cout<<"sahgoi"<<endl;
  
  
  for(int i=0;i<262044;i++){
    data123[i] = i;
  }
  
  readFile(file);
  preprocess(); // all input has been read and stored in iset.
  //cout<<maxInstructions<<endl;
  //cout<<currentInstNum<<endl;
  
  //cout<<numberofInst<<endl;
  //maxInstructions = currentInstNum;
    //for(int i=0;i<inputprogram.size();i++){
      //cout<<inputprogram[i]<<endl;
  //}
  maxInstructions=numberofInst;
//   for(int i=0;i<iset.size();i++){
//       instruction x=iset[i];
//       vector<int> y=x.cirs;
//       int id=x.opID;
//       cout<<y[0]<<","<<y[1]<<","<<y[2]<<","<<id<<endl;
//   }
  
  execute ();
  //cout<<pc<<endl;
  cout<<error<<endl;;

  return 0;
}