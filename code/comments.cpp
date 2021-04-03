

// some error in this function. Copied preprocess from Abhishek's file.
/*
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
*/

// This was Assignment-3 execute function which was
// modified quite a lot for minor exam.

/*
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
*/

// This was main function of Assignment-3.

/*
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
  
  //for(int i=250-numberofInst;i<250-numberofInst+10;i++) data123[i] = i-(250-numberofInst);
  
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
  
  for(int i=numberofInst;i<(1<<18);i++){
    if(data123[i-numberofInst]!=0) {
      cout<<(i)*4<<"-"<<(i)*4+3<<": "<<hex<<data123[i-numberofInst]<<dec<<endl;
      outstream<<(i)*4<<"-"<<(i)*4+3<<": "<<hex<<data123[i-numberofInst]<<dec<<endl;
    }
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
*/