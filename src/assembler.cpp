#include "../inc/assembler.hpp"
#include "../inc/regex.hpp"
#include <iomanip>
#include <bitset>

string Assembler::optionArg="";
string Assembler::cmdOutputFile="";
string Assembler::cmdInputFile =""; 
string Assembler::currentLine="";
string Assembler::cleanCurrentLine="";
int Assembler::symbolId=0;
int Assembler::sectionNumber=0; 
long Assembler::locationCounter=0; 
int Assembler::currentSectionNumber=0; 
string Assembler::currentSectionName=""; 

using namespace std; 


bool Assembler::checkCmdArguments(int argc, char* argv[]){
  
  for (int i = 0; i < argc; i++){
    //not enough arguments 
    if(!argv[i])throw BadCmdArgsException(); 
  }
  optionArg = argv[1]; 
  cmdOutputFile = argv[2]; 
  cmdInputFile = argv[3]; 

  std::cout<<cmdInputFile<<"\n"<<cmdOutputFile<<"\n"; 

  if(std::regex_match(optionArg, optionArgRegex) 
  && std::regex_match(cmdInputFile, inputFileRegex)
  && std::regex_match(cmdOutputFile, outputFileRegex)){
    std::cout<<"Lepo zadati argumenti \n"; 
    return true; 
  }
  
  throw BadCmdArgsException(); 
}
    
void Assembler::openParseFile(){
  fstream inFile;
  inFile.open(inputFile); 
  if(!inFile.is_open())throw FileNotOpenException();
  cout<<"File opened! \n";  
  currLineNum=1; 

  while(getline(inFile, currentLine)){
    if(endFlag)break; 
    instrDirNum=0; 
    smatch regexMatch;
    if(regex_search(cleanCurrentLine, regexMatch, emptyLineRegex))
      continue; //if empty line skip iteration

    formatLine(); 

    bool labelLine;
    labelLine = checkIfLabel(cleanCurrentLine);
    if(labelLine){
      //remove label from current line, instruction could be after label
      cleanCurrentLine=regex_replace(cleanCurrentLine, labelRegex,"");
      cout<<"label line after shortening \n";
      cout<<cleanCurrentLine<<endl; 
      if(regex_search(cleanCurrentLine, regexMatch, emptyLineRegex))
        continue; //if empty line skip iteration
    }

    cleanLines.push_back(cleanCurrentLine);  
    currLineNum++; 

    if(checkIfExternDirective(cleanCurrentLine))continue; 
    if(checkIfGlobalDirective(cleanCurrentLine))continue; 
    if(checkIfSectionDirective(cleanCurrentLine))continue; 
    if(checkIfWordDirective(cleanCurrentLine))continue; 
    if(checkIfSkipDirective(cleanCurrentLine))continue; 
    if(checkIfEndDirective(cleanCurrentLine))continue;
    if(checkIfInstruction(cleanCurrentLine))continue;  
    //check if instruction 

    //throw instruction/directive not found 

  }
  if(!endFlag){
    string msg ="End directive doesn't exist! Error at line: "+ to_string(currLineNum);
    throw BadSynataxException(msg);
  }
  cout<<"TABELA SIMBOLA:"<<endl; 
  // cout<<"name \t isGlobal \t id \t section \t size"<<endl;
  cout<< left<< setw(14)<<setfill(' ')<<"symbolName";
  cout<< left<< setw(14)<<setfill(' ')<<"isGlobal";
  cout<< left<< setw(14)<<setfill(' ')<<"symbolID";
  cout<< left<< setw(14)<<setfill(' ')<<"sectionNum";
  cout<< left<< setw(14)<<setfill(' ')<<"value";
  cout<< left<< setw(14)<<setfill(' ')<<"size";
  cout<<endl; 
  for (auto const& x : symbolTable){
    cout<< left<< setw(14)<<setfill(' ')<<x.second.symbolName;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.isGlobal;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.symbolId;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.sectionNum;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.value;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.size;
    cout<<endl; 

    // cout<<x.second.symbolName<<"\t"<<x.second.isGlobal<<"\t"<<x.second.symbolId<<"\t"<<x.second.sectionNum<<"\t"<<x.second.size<<endl;
  }
  cout<<endl; 
  // cout<<"TABELA KORISCENJA:"<<endl; 
  //   cout<< left<< setw(14)<<setfill(' ')<<"symbol";
  //   cout<< left<< setw(14)<<setfill(' ')<<"address";
  //   cout<< left<< setw(14)<<setfill(' ')<<"section";
  //   cout<< left<< setw(14)<<setfill(' ')<<"type";
  //   cout<<endl; 

  // for (auto const& x : symbolTable){
  //   for(auto u: x.second.useVector){
  //     // cout<<x.first<<": "<<u.address<<" "<<u.section<<" "<<u.type<<endl;
  //     cout<< left<< setw(14)<<setfill(' ')<<x.first;
  //     cout<< left<< setw(14)<<setfill(' ')<<u.address;
  //     cout<< left<< setw(14)<<setfill(' ')<<u.section;
  //     cout<< left<< setw(14)<<setfill(' ')<<u.type;
  //     cout<<endl; 

  //   }
  // }

  cout<<"RELOKACIONI ZAPISI SEKCIJE:"<<endl; 
  for(auto s:sectionTable){
    cout<<"sekcija: "<<s.second.sectionName<<endl; 
    cout<< left<< setw(14)<<setfill(' ')<<"offset";
    cout<< left<< setw(14)<<setfill(' ')<<"type";
    cout<< left<< setw(14)<<setfill(' ')<<"symbol";
    cout<< left<< setw(14)<<setfill(' ')<<"addend";
    cout<<endl; 
    for(auto r:s.second.sectionRelocations){
      cout<< left<< setw(14)<<setfill(' ')<<r.offset;
      cout<< left<< setw(14)<<setfill(' ')<<r.type;
      cout<< left<< setw(14)<<setfill(' ')<<r.symbol;
      cout<< left<< setw(14)<<setfill(' ')<<r.addend;
      cout<<endl; 
    }
    cout<<endl; 
  }

  cout<<"KOD SEKCIJA:"<<endl;
  int cnt=1; 
  int cntNewLine=1; 
  for(auto s:sectionTable){
    cout<<"Ime sekcije: "<<s.first<<endl; 
    cntNewLine=1; 
    for(auto c: s.second.code){
      cout<<c; 
      if(cnt==2){
        cout<<" ";
        cnt=0; 
      }
      if(cntNewLine==20){
        cout<<endl;
        cntNewLine=0; 
      }
      cnt++; 
      cntNewLine++; 
    }
  cout<<endl;
  }

}

void Assembler::formatLine(){
    smatch regexMatch;
    cleanCurrentLine = currentLine; 
    if(regex_search(cleanCurrentLine, regexMatch, commentRegex)){
      // cout<<"comments \n"; 
      // cout<<cleanCurrentLine<<"\n";//before replacement
      //remove all comments 
      cleanCurrentLine=regex_replace(cleanCurrentLine, commentRegex, ""); 
      // cout<<cleanCurrentLine<<"\n";//after replacement 
    }
    // cout<<"whitespaces \n"; 
    // cout<<cleanCurrentLine<<"\n";//before replacement
  
    //remove all unnecessary whitespaces
    cleanCurrentLine = regex_replace(cleanCurrentLine, startLineSpaceRegex, ""); 

    cleanCurrentLine = regex_replace(cleanCurrentLine, endLineSpaceRegex, ""); 
    //remove mulitple spaces
    cleanCurrentLine = regex_replace(cleanCurrentLine, additionalSpaceRegex, " "); 
    //remove none or multiple spaces after comma
    cleanCurrentLine = regex_replace(cleanCurrentLine, commaSpaceRegex, ", "); 
    // cout<<cleanCurrentLine<<"\n"; 

    //check if label is well written 
    if(regex_search(cleanCurrentLine, regexMatch, somethingBeforeLabelRegex)){
      throw BadInputFileSyntax(currLineNum); 
    }


}

bool Assembler::checkIfLabel(string currLine){
  smatch regexMatch; 
  if(regex_search(currLine, regexMatch, labelRegex)){
    cout<<"label \n";
    cout<<currLine<<"\n"; 
    processLabel(currLine); 
    return true; 
  }
  return false; 
}

bool Assembler::checkIfExternDirective(string currLine){
  smatch regexMatch; 
  if(regex_search(currLine, regexMatch, externRegex)){
    if(instrDirNum>0)throw BadInputFileSyntax(currLineNum);
    instrDirNum++; 
    cout<<"extern \n";
    cout<<currLine<<"\n"; 
    processExternDirective(currLine); 
    return true; 
  }
  return false;
}
bool Assembler::checkIfGlobalDirective(string currLine){
  smatch regexMatch; 
  if(regex_search(currLine, regexMatch, globalRegex)){
    if(instrDirNum>0)throw BadInputFileSyntax(currLineNum);
    instrDirNum++; 
    cout<<"global \n";
    cout<<currLine<<"\n"; 
    processGlobalDirective(currLine); 
    return true; 
  }
  return false; 
}
bool Assembler::checkIfEndDirective(string currLine){
  smatch regexMatch; 
  if(regex_search(currLine, regexMatch, endRegex)){
    if(instrDirNum>0)throw BadInputFileSyntax(currLineNum);
    instrDirNum++; 
    cout<<"end \n";
    cout<<currLine<<"\n"; 
    endFlag=true; 
    symbolTable[currentSectionName].size=locationCounter; 
    return true; 
  }  
  return false; 
}
bool Assembler::checkIfSectionDirective(string currLine){
  smatch regexMatch; 
  if(regex_search(currLine, regexMatch, sectionRegex)){
    if(instrDirNum>0)throw BadInputFileSyntax(currLineNum);
    instrDirNum++; 
    cout<<"section \n";
    cout<<currLine<<"\n"; 
    processSectionDirective(currLine); 
    return true; 
  } 
  return false; 
}
bool Assembler::checkIfSkipDirective(string currLine){
  smatch regexMatch; 
  if(regex_search(currLine, regexMatch, skipRegex)){
    if(instrDirNum>0)throw BadInputFileSyntax(currLineNum);
    instrDirNum++; 
    cout<<"skip \n";
    cout<<currLine<<"\n"; 
    processSkipDirective(currLine); 
    return true; 
  } 
  return false; 
}
bool Assembler::checkIfWordDirective(string currLine){
  smatch regexMatch; 
  if(regex_search(currLine, regexMatch, wordRegex)){
    if(instrDirNum>0)throw BadInputFileSyntax(currLineNum);
    instrDirNum++; 
    cout<<"word \n";
    cout<<currLine<<"\n"; 
    processWordDirective(currLine); 
    return true; 
  } 
  return false; 
}

bool Assembler::checkIfInstruction(string currLine){
  smatch match; 
  if(regex_search(currLine, match, noOperandRegex)){
    string operation = match[0]; 
    cout<<"OPERATION IS "+operation<<endl; 
    string code; 
    if(operation=="halt"){
      code="00"; 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=1; 
    }
    else if(operation=="iret"){
      code="20"; 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=1; 
    }
    else if(operation=="ret"){
      code="40"; 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=1; 
    }
  }
  else if(regex_search(currLine, match, registersOnlyOneOperandRegex)){

    //tokenize the line 
    char* line = new char[currLine.length()+1]; 
    strcpy(line, currLine.c_str()); 
    char * token;
    token = strtok(line," ,");
    vector<string>tokens; 
    while (token != NULL){
      tokens.push_back(token); 
      token = strtok (NULL, " ,");
    }
    string operation = tokens[0]; 
    cout<<endl<<"OPERATION IS "+operation<<endl; 
    string code; 


    if(operation=="int"){
      
      cout<<"OPERANDS ARE: "; 
      cout<<tokens[1]<<endl; 

      string binary_reg = registerCode(tokens[1]); 
      cout<<"hex reg: "<<binary_reg<<endl; 
      code="10";
      code+=binary_reg;
      code+="F"; 
      cout<<"code for int instruction: "<<code<<endl;

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;  

    }
    else if(operation=="push"){
      //treat it as str

      cout<<"push - reg indirektno"<<endl;
      cout<<tokens[1]; 
      cout<<endl;

      code="B0"; 

      string reg_code2 = registerCode(tokens[1]);
      code.append("6");//RD is sp 
      code.append(reg_code2); //RS
      code.append("1");//UP predecrement 
      code.append("2"); //AM
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=3;  
      
    }
    else if(operation=="pop"){
      //treat it as ldr
      //postincrement 
      cout<<"pop - reg indirektno"<<endl;
      cout<<tokens[1]; 
      cout<<endl;

      code="A0"; 

      string reg_code2 = registerCode(tokens[1]);
      code.append("6");//RD is sp 
      code.append(reg_code2); //RS
      code.append("4");//UP postincrement 
      code.append("2"); //AM
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=3;
      
    }
    else if(operation=="not"){
      cout<<"OPERANDS ARE: "; 
      cout<<tokens[1]<<endl; 

      string binary_reg = registerCode(tokens[1]); 
      cout<<"hex reg: "<<binary_reg<<endl; 
      code="80";
      code+=binary_reg;
      code+=binary_reg; 
      cout<<"code for not instruction: "<<code<<endl;

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
    }
  }
  else if(regex_search(currLine, match, registersTwoOperandRegex)){
       //tokenize the line 
    char* line = new char[currLine.length()+1]; 
    strcpy(line, currLine.c_str()); 
    char * token;
    token = strtok(line," ,");
    vector<string>tokens; 
    while (token != NULL){
      tokens.push_back(token); 
      token = strtok (NULL, " ,");
    }
    string operation = tokens[0]; 
    cout<<"OPERATION IS "+operation<<endl; 
    string code; 

    string reg1 = registerCode(tokens[1]);
    string reg2 = registerCode(tokens[2]);
    
    if(operation=="xchg"){
      code="60";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for xchg instruction: "<<code<<endl; 
      
    }
    else if(operation=="add"){
      code="70";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for add instruction: "<<code<<endl; 
    }
    else if(operation=="sub"){
      code="71";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for sub instruction: "<<code<<endl;
    }
    else if(operation=="mul"){
      code="72";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for mul instruction: "<<code<<endl;
    }
    else if(operation=="div"){
      code="73";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for div instruction: "<<code<<endl;
    }
    else if(operation=="cmp"){
      code="74";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for cmp instruction: "<<code<<endl;
    }
    else if(operation=="and"){
      code="81";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for and instruction: "<<code<<endl;
    }
    else if(operation=="or"){
      code="82";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for or instruction: "<<code<<endl;
    }
    else if(operation=="xor"){
      code="83";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for xor instruction: "<<code<<endl;
    }
    else if(operation=="test"){
      code="84";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for test instruction: "<<code<<endl;
    }
    else if(operation=="shl"){
      code="90";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for shl instruction: "<<code<<endl;
    }
    else if(operation=="shr"){
      code="91";
      code+=reg1;
      code+=reg2; 

      //pushing into code 
      for(int i=0; i<code.size(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=2;
      cout<<"Code for shr instruction: "<<code<<endl;
    }

  }
  else{
    processInstruction(currLine); 
  }
  return true; 

}

void Assembler::processLabel(string currLine){
  regex symbolNameRegex("[a-zA-Z][a-zA-Z0-9_]*:"); 
  smatch regexMatch;
  string symbolName; 
  if(regex_search(currLine, regexMatch, symbolNameRegex)){
    symbolName=regexMatch[0];
    cout<<"symbol name is: "<<symbolName<<"\n"; 
  }
  symbolName = symbolName.substr(0, currLine.size()-1);//to remove ':'
  cout<<"only symbol name: "+symbolName+"\n"; 
  
  bool found = false; 
  if(symbolTable.find(symbolName)!=symbolTable.end()){
      cout<<"symbol found in symbol table"<<endl; 
      found=true; 
      symbolTable[symbolName].isDefined=true; 
      symbolTable[symbolName].sectionNum=currentSectionNumber; 
      symbolTable[symbolName].value=locationCounter; 

      // symbolUseEntry symbUse; 
      // symbUse.address = locationCounter; 
      // symbUse.section = currentSectionNumber; 
      // symbUse.type = 0; 
      // symbolTable[symbolName].useVector.push_back(symbUse); 
  }
  else{
    symbolTable[symbolName].isDefined=true; 
    symbolTable[symbolName].isGlobal=false; 
    symbolTable[symbolName].sectionNum=currentSectionNumber; 
    symbolTable[symbolName].size=-1;
    symbolTable[symbolName].symbolId=symbolId;
    symbolTable[symbolName].symbolName=symbolName;
    symbolTable[symbolName].value=locationCounter;
    symbolId++;
    cout<<"simbol dodat u tabelu simbola"<<endl; 

    // symbolUseEntry symbUse; 
    // symbUse.address = locationCounter; 
    // symbUse.section = currentSectionNumber; 
    // symbUse.type = 0; 
    // symbolTable[symbolName].useVector.push_back(symbUse); 
  }


}

void Assembler::processGlobalDirective(string currLine){
  regex globalRgx(".global ");
  currLine = regex_replace(currLine, globalRgx, ""); 
  cout<<"after cutting global \n"<<currLine<<endl; 
  vector<string> symbols; 
  smatch match; 
  while(regex_search(currLine, match, symbolRegex)){
    symbols.push_back(match.str(0));
    currLine = match.suffix().str(); 
  }
  cout<<"Symbols from symbol list:"<<endl; 
  for(auto s:symbols){
    cout<<s<<endl; 
  }
  cout<<"------------------"<<endl; 
  for(auto s:symbols){
    if(sectionTable.find(s)!=sectionTable.end()){
      string msg ="Section cant't be global! Error at line: "+ to_string(currLineNum);
      throw BadSynataxException(msg);
    }
    if(symbolTable.find(s)==symbolTable.end()){
      //if the symbol is not in symbol table
      //add it to symbol table 
      symbolTable[s].isDefined=false; 
      symbolTable[s].isGlobal=true; 
      symbolTable[s].sectionNum=currentSectionNumber; 
      symbolTable[s].size=-1;
      symbolTable[s].symbolId=symbolId;
      symbolTable[s].symbolName=s;
      symbolTable[s].value=0;
      symbolId++;
      cout<<"symbol "<<s<<" added to the symbol table"<<endl; 

      //make symbol use entry 
      // symbolUseEntry symbUse; 
      // symbUse.address = locationCounter; 
      // symbUse.section = currentSectionNumber; 
      // symbUse.type = 0; 
      // symbolTable[s].useVector.push_back(symbUse); 

    }
    else if(symbolTable[symbol].sectionNum == 0){
      string msg ="Extern symbol can't be global at the same time! Error at line: "+ to_string(currLineNum);
      throw BadSynataxException(msg);
    }
    else{
      symbolTable[s].isGlobal=true; 
    }

  }

}

void Assembler::processExternDirective(string currLine){
  regex externRgx(".extern ");
  currLine = regex_replace(currLine, externRgx, ""); 
  cout<<"after cutting extern \n"<<currLine<<endl; 
  vector<string> symbols; 
  smatch match; 
  while(regex_search(currLine, match, symbolRegex)){
    symbols.push_back(match.str(0));
    currLine = match.suffix().str(); 
  }
  cout<<"Symbols from symbol list:"<<endl; 
  for(auto s:symbols){
    cout<<s<<endl; 
  }
  cout<<"------------------"<<endl; 
  
  for(auto s:symbols){
    //check if symbol is already defined 
    if(symbolTable.find(s)!=symbolTable.end() && symbolTable[s].isDefined==true){
      string msg ="Symbol already defined! Error at line: "+ to_string(currLineNum);
      throw BadSynataxException(msg);
    }
    else if(symbolTable.find(s)!=symbolTable.end() && symbolTable[s].sectionNum>0){
      string msg ="Global symbol can't be extern at the same time! Error at line: "+ to_string(currLineNum);
      throw BadSynataxException(msg);
    }
    else if(symbolTable.find(s)!=symbolTable.end()){  
      symbolTable[s].isGlobal=true; 
      symbolTable[s].sectionNum=0; 
    } 
    else if(symbolTable.find(s)==symbolTable.end()){
      //add symbol to symbol table 
      symbolTable[s].isDefined=false; 
      symbolTable[s].isGlobal=true; 
      symbolTable[s].sectionNum=0; //no section 
      symbolTable[s].size=-1;
      symbolTable[s].symbolId=symbolId;
      symbolTable[s].symbolName=s;
      symbolTable[s].value=0;
      symbolId++;
      cout<<"symbol "<<s<<" added to the symbol table"<<endl; 
    }
  }
}

void Assembler::processSectionDirective(string currLine){
  char* line = new char[currLine.length()+1]; 
  strcpy(line, currLine.c_str()); 
  char* sectionLabel = strtok(line, " "); 
  char* sectionName = strtok(NULL, " "); 
  cout<<"section name is: "<<sectionName<<"\n"; 

  
  //check if section is in symbolTable 
  if(symbolTable.find(sectionName)!=symbolTable.end() && symbolTable[sectionName].size==-1){
    //section exists in symbolTable 
    for(auto symb: symbolTable){
      if(symb.second.symbolId==currentSectionNumber){
        symb.second.size=locationCounter; 
        locationCounter=symbolTable[sectionName].size; //set lc to size of section that continues
        break; 
      }
    }
  }
  else if(symbolTable.find(sectionName)!=symbolTable.end() && symbolTable[sectionName].size!=-1){
    //error: section already exists as a symbol
      string msg ="Section already exists as a symbol! Error at line: "+ to_string(currLineNum);
      throw BadSynataxException(msg);
  }
  else{
    //section does not exist in symbol table 
    //previous section closed, set its size 
    for(auto symb: symbolTable){
      if(symb.second.symbolId==currentSectionNumber){
        symb.second.size=locationCounter; 
        locationCounter=0; 
        break; 
      }
    }

    //new section opened 
    currentSectionNumber=symbolId;
    currentSectionName = sectionName; 
    locationCounter=0;  
    //create symbol table entry 
    symbolTable[sectionName].isDefined=true; 
    symbolTable[sectionName].isGlobal=true; 
    symbolTable[sectionName].sectionNum=currentSectionNumber; 
    symbolTable[sectionName].size=0;
    symbolTable[sectionName].symbolId=symbolId;
    symbolTable[sectionName].symbolName=sectionName;
    symbolTable[sectionName].value=0;
    symbolId++;
    cout<<"section "<<sectionName<<" added to the symbol table"<<endl;

    //create section table entry 
    sectionTable[sectionName].sectionId=symbolId; 
    sectionTable[sectionName].sectionName=sectionName; 
    sectionTable[sectionName].size=0; 


  }

}

void Assembler::processSkipDirective(string currLine){
  char* line = new char[currLine.length()+1]; 
  strcpy(line, currLine.c_str()); 
  char* skipLabel = strtok(line, " "); 
  char* literal = strtok(NULL, " "); 
  char* pEnd; 
  long bytes; 
  if(literal[0]=='0'){
    //hexadecimal literal
    bytes = strtol(literal, &pEnd, 16);
    cout<<"skip literal is: "<<bytes<<"\n"; 
  }
  else{
    //decimal literal 
    bytes = strtol(literal, &pEnd, 10);
    cout<<"skip literal is: "<<bytes<<"\n"; 
  }

  for(int i=0; i<bytes*2; i++){
    sectionTable[currentSectionName].code.push_back('0'); 
  }
  string section = findSectionName(); 
  sectionTable[section].size+=bytes;
  symbolTable[section].size+=bytes; 
  locationCounter+=bytes; 

}


void Assembler::processWordDirective(string currLine){
  //check if section is opened 
  if(currentSectionNumber==-1){
    string msg ="Section not opened! Error at line: "+ to_string(currLineNum);
    throw BadSynataxException(msg);
  }

  vector<string>literals; 

  regex wordRgx(".word ");
  currLine = regex_replace(currLine, wordRgx, ""); 
  cout<<"after cutting word \n"<<currLine<<endl; 
  vector<string> symbols; 

  char* line = new char[currLine.length()+1]; 
  strcpy(line, currLine.c_str()); 
  char* sym = strtok(line, " ,"); 
  while(sym!=NULL){
    string token(sym); 
    smatch regex_match; 
    if(regex_search(token, regex_match, literalRegex)){
      //literal parsed 
      literals.push_back(token);
      int lit = stoi(token); 
      //literal must be withing range of 2 bytes 
      if(lit>WORD_MAX || lit<WORD_MIN){
        string msg ="Literal out of range! Error at line: "+ to_string(currLineNum);
        throw BadSynataxException(msg);
      }
      
      //TODO add hex regex 
      regex hexRegex("0x[0-9A-Fa-f]+"); 
      smatch sm; 
      if(regex_search(token, sm, hexRegex)){
        //hex literal 
        
      }
      else{
        //dec literal 
        //convert dec to hex 
        token = decToHex(stoi(token)); 
      }
      cout<<"Token is: "<<token<<endl; 
      regex hexPrefix("0x"); 
      token=regex_replace(token, hexPrefix, ""); 

      //find cur section name via cur section number
      string section = findSectionName(); 


      for(int i=token.length();i<4;i++){
        sectionTable[section].code.push_back('0'); //add leading zeros 
      }
      for(int i=0; i<token.length(); i++){
        sectionTable[section].code.push_back(token[i]); 
      }
      sectionTable[section].size+=2;
      symbolTable[section].size+=2; 
      locationCounter+=2; 
    }
    else{
      //symbol parsed 
      symbols.push_back(token); 

      //check if symbol is not in symbol table 
      if(symbolTable.find(token)==symbolTable.end()){
        //add symbol to symbol table 
        symbolTable[token].isDefined=false; 
        symbolTable[token].isGlobal=false; 
        symbolTable[token].sectionNum=currentSectionNumber; //no section 
        symbolTable[token].size=-1;
        symbolTable[token].symbolId=symbolId;
        symbolTable[token].symbolName=token;
        symbolTable[token].value=0;
        symbolId++;
        cout<<"symbol "<<token<<" added to the symbol table"<<endl; 
        string section = findSectionName(); 
        for(int i=0;i<4;i++){
          sectionTable[section].code.push_back('0');
        }

        //add symbol to symbol use entry 
        //make symbol use entry 
        symbolUseEntry symbUse; 
        symbUse.address = locationCounter; 
        symbUse.section = currentSectionNumber; 
        symbUse.type = 0; 
        symbolTable[token].useVector.push_back(symbUse); 

        //TODO make relocation entry 
        addRelocation(locationCounter, 0, symbolTable[token].symbolId, 0, currentSectionName); 

        sectionTable[section].size+=2;
        symbolTable[section].size+=2; 
        locationCounter+=2; 

      }
      else{
        //symbol found in symbol table 
        if(symbolTable[token].isGlobal==false){
          //symbol is local
          string value = to_string(symbolTable[token].value);
          string hex_value = literalToHex(value); 
          // regex hexPrefix("0x"); 
          // hex_value=regex_replace(hex_value, hexPrefix, ""); 
          
          string section = findSectionName(); 


          for(int i=hex_value.length();i<4;i++){
            hex_value.insert(0,"0"); //add leading zeros 
          }
          cout<<"big endian value: "+hex_value<<endl; 
          //make it little endian - rotate bytes in symbValue
          string tmp=hex_value.substr(0,2);
          hex_value = hex_value.substr(2,2); 
          hex_value.append(tmp); 
          cout<<"little endian value: "+hex_value<<endl; 

          for(int i=0; i<hex_value.length(); i++){
            sectionTable[section].code.push_back(hex_value[i]); 
          }
          //add symbol to symbol use entry 
          //make symbol use entry 
          symbolUseEntry symbUse; 
          symbUse.address = locationCounter; 
          symbUse.section = currentSectionNumber; 
          symbUse.type = 0; 
          symbolTable[token].useVector.push_back(symbUse); 

          //TODO make relocation entry 
          addRelocation(locationCounter, 0, symbolTable[token].symbolId, 0, currentSectionName);


          sectionTable[section].size+=2;
          symbolTable[section].size+=2; 
          locationCounter+=2; 

        }
        else{
          //symbol is global 
          string section = findSectionName(); 

          for(int i=0;i<4;i++){
            sectionTable[section].code.push_back('0');
          }
          //add symbol to symbol use entry 
          //make symbol use entry 
          symbolUseEntry symbUse; 
          symbUse.address = locationCounter; 
          symbUse.section = currentSectionNumber; 
          symbUse.type = 0; 
          symbolTable[token].useVector.push_back(symbUse); 

          //TODO make relocation entry 
          addRelocation(locationCounter, 0, symbolTable[token].symbolId, 0, currentSectionName); 

          sectionTable[section].size+=2;
          symbolTable[section].size+=2; 
          locationCounter+=2; 

        }
      }
    }
    sym = strtok(NULL, " ,"); 
  }

  cout<<"word symbols:"<<endl; 
  for(auto s:symbols){
    cout<<s<<endl; 
  }
  
  cout<<"word literals:"<<endl; 
  for(auto l:literals){
    cout<<l<<endl; 
  }
  cout<<endl; 

  cout<<"CODE:"<<endl; 
  for(auto c:code){
    cout<<c; 
  }
  cout<<endl; 

}


string Assembler::decToHex(int dec){
  std::stringstream ss;
  ss<< std::hex << dec; // int decimal_value
  std::string res ( ss.str() );
  return res; 
}

int Assembler::hexToDec(string hex){
  int decimal_value; 
  std::stringstream ss;
  ss  << hex; // std::string hex_value
  ss >> std::hex >> decimal_value ; //int decimal_value

  return decimal_value; 
}

string Assembler::findSectionName(){
  for(auto s:symbolTable){
    if(s.second.symbolId==currentSectionNumber)
      return s.second.symbolName; 
  }
  return nullptr; 
} 

string Assembler::decToBin(string dec){
  int dec_num = stoi(dec);
  std::string binary = std::bitset<4>(dec_num).to_string(); //to binary
  return binary; 
}

string Assembler::registerCode(string reg){

  if(reg=="sp")return "6"; 
  else if(reg=="pc")return "7"; 
  else if(reg=="psw")return "8"; 
  else return reg.substr(1,1); 
  return ""; 

}

void Assembler::addRelocation(int offset, int type, int symbol, int addend, string currSectionName){
   relocationEntry reloc;
   reloc.offset=offset; 
   reloc.type=type; 
   reloc.symbol=symbol; 
   reloc.addend=addend; 
   sectionTable[currentSectionName].sectionRelocations.push_back(reloc);  

}


void Assembler::processInstruction(string currLine){
  // cout<<"Process instrucion line:"<<endl<<currLine<<endl; 
  smatch match; 
  if(regex_search(currLine, match, ldrStrRegex)){
    cout<<endl<<"Current line: "<<currLine<<endl;
    vector<string>tokens; 

    char* line = new char[currLine.length()+1]; 
    strcpy(line, currLine.c_str()); 
    char * token;
    token = strtok(line," ,[]+");
    while (token != NULL){
      tokens.push_back(token); 
      token = strtok (NULL, " ,[]+");
    }

    cout<<"Lista tokena iz ldr/str:"<<endl; 
    for(auto t:tokens){
      cout<<t<<endl; 
    }

    string operation = tokens[0]; 
    string reg = tokens[1]; 
    string operand = tokens[2]; 
    string code; 

    if(regex_search(operand, match, ldrStrImmediateRegex)){
      cout<<"neposredno"<<endl;
      cout<<operand;
      cout<<endl;

      if(operation=="ldr")code="A0";
      else if(operation=="str")code="B0";

      string reg_code = registerCode(reg); 
      code+=reg_code;//RD
      code+="0";//RS
      code+="0";//UP
      code+="0"; //AM

      regex symbRgx("^[a-zA-Z][a-zA-Z0-9_]*$"); 
      operand=operand.substr(1,operand.length());
      //check if operand is symbol
      if(regex_search(operand, match, symbRgx)){
        //operand is symbol
        cout<<"operand is symbol"<<endl; 
        //make relocation entry 
        string value = processSymbol(operand, locationCounter+3);
        code.append(value); 
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      } 
      else{
        //operand is literal
        cout<<"operand is literal"<<endl; 
        operand = literalToHex(operand); 
        cout<<"operand: "<<operand<<endl;
        for(int i=operand.length(); i<4; i++){
          code+="0"; 
        }
        for(int i=0; i<operand.length(); i++){
          code+=operand[i]; 
        }
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }
    }
    else if(regex_search(currLine, match, ldrStrRegIndRegex)){
      cout<<"reg indirektno"<<endl;
      cout<<operand; 
      cout<<endl;

      if(operation=="ldr")code="A0";
      else if(operation=="str")code="B0";

      string reg_code = registerCode(reg); 
      string reg_code2 = registerCode(operand);
      code.append(reg_code);//RD
      code.append(reg_code2); //RS
      code.append("0");//UP
      code.append("2"); //AM
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=3;      


    }
    else if(regex_search(currLine, match, ldrStrRegIndDispRegex)){
      cout<<"reg indirektno sa pomerajem"<<endl;
      cout<<tokens[2]<<" "<<tokens[3]; 
      cout<<endl;

      if(operation=="ldr")code="A0";
      else if(operation=="str")code="B0";

      string reg_code = registerCode(tokens[1]); 
      string reg_code2 = registerCode(tokens[2]);
      code.append(reg_code);//RD
      code.append(reg_code2); //RS
      code.append("0");//UP
      code.append("3"); //AM

      regex symbRgx("^[a-zA-Z][a-zA-Z0-9_]*$"); 
      //check if operand is symbol
      if(regex_search(tokens[3], match, symbRgx)){
        //operand is symbol
        cout<<"operand is symbol"<<endl; 
        //make relocation entry 
        string value = processSymbol(tokens[3], locationCounter+3);
        code.append(value); 
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }
      else{
        //operand is literal
        cout<<"operand is literal"<<endl; 
        operand = literalToHex(tokens[3]); 
        cout<<"operand: "<<operand<<endl;
        for(int i=operand.length(); i<4; i++){
          code.append("0"); 
        }
        for(int i=0; i<operand.length(); i++){
          code+=operand[i]; 
        }
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }



    }
    else if(regex_search(operand, match, ldrStrRegDirRegex)){
      cout<<"reg direktno"<<endl;
      cout<<operand; 
      cout<<endl;

      if(operation=="ldr")code="A0";
      else if(operation=="str")code="B0";

      string reg_code = registerCode(reg); 
      string reg_code2 = registerCode(operand);
      code.append(reg_code);//RD
      code.append(reg_code2); //RS
      code.append("0");//UP
      code.append("1"); //AM
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=3;

    }
    else if(regex_search(operand, match, ldrStrMemDirRegex)){
      cout<<"mem direktno"<<endl;
      cout<<operand; 
      cout<<endl;

      if(operation=="ldr")code="A0";
      else if(operation=="str")code="B0";

      string reg_code = registerCode(reg); 
      code.append(reg_code);//RD
      code.append("0"); //RS
      code.append("0");//UP
      code.append("4"); //AM
      regex symbRgx("^[a-zA-Z][a-zA-Z0-9_]*$"); 
      //check if operand is symbol
      if(regex_search(operand, match, symbRgx)){
        //operand is symbol
        cout<<"operand is symbol"<<endl; 
        //make relocation entry 
        string value = processSymbol(operand, locationCounter+3);
        code.append(value); 
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      } 
      else{
        //operand is literal
        cout<<"operand is literal"<<endl; 
        operand = literalToHex(operand); 
        cout<<"operand: "<<operand<<endl;
        for(int i=operand.length(); i<4; i++){
          code.append("0"); 
        }
        for(int i=0; i<operand.length(); i++){
          code+=operand[i]; 
        }
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }


    }
    else if(regex_search(operand, match, ldrStrPcRelRegex)){
      cout<<"pc relativno"<<endl;
      cout<<operand; 
      cout<<endl;

      if(operation=="ldr")code="A0";
      else if(operation=="str")code="B0";

      string reg_code = registerCode(reg); 
      code.append(reg_code); 
      code.append("7"); //RS is pc reg 
      code.append("03"); //UP AM 
      operand = operand.substr(1, operand.length()); 

      string value = processSymbolForRelocation(operand, locationCounter);
      code.append(value); 
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=5;

    }

  }
  else if(regex_search(currLine, match, jmpRegex)){
    //jmp type instruction 
    cout<<endl<<"Current line: "<<currLine<<endl;
    vector<string>tokens; 

    char* line = new char[currLine.length()+1]; 
    strcpy(line, currLine.c_str()); 
    char * token;
    token = strtok(line," ,[]+*%");
    while (token != NULL){
      tokens.push_back(token); 
      token = strtok (NULL, " ,[]+*%");
    }

    cout<<"Lista tokena iz ldr/str:"<<endl; 
    for(auto t:tokens){
      cout<<t<<endl; 
    }

    string operation = tokens[0]; 
    regex instrOnly("^(jmp|call|jeq|jne|jgt)[ ]+"); 
    string operand = regex_replace(currLine, instrOnly, "");
    cout<<"operand: "<<operand<<endl;
    string code; 
    //instrDesc
    if(operation=="jmp")code.append("50");
    else if(operation=="jeq")code.append("51");
    else if(operation=="jne")code.append("52");
    else if(operation=="jgt")code.append("53");
    else if(operation=="call")code.append("30"); 


    if(regex_search(operand, match, jmpImmediateRegex)){
      cout<<"neposredno"<<endl;
      cout<<operand;
      cout<<endl;

      code.append("F0");//RD RS 
      code.append("00"); //UP AM

      regex symbRgx("^[a-zA-Z][a-zA-Z0-9_]*$"); 
      //check if operand is symbol
      if(regex_search(operand, match, symbRgx)){
        //operand is symbol
        cout<<"operand is symbol"<<endl; 
        //make relocation entry 
        string value = processSymbol(operand, locationCounter+3);
        code.append(value); 
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      } 
      else{
        //operand is literal
        cout<<"operand is literal"<<endl; 
        operand = literalToHex(operand); 
        cout<<"operand: "<<operand<<endl;
        for(int i=operand.length(); i<4; i++){
          code+="0"; 
        }
        for(int i=0; i<operand.length(); i++){
          code+=operand[i]; 
        }
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }

    }
    else if(regex_search(operand, match, jmpRegDirRegex)){
      cout<<"reg direktno"<<endl;
      cout<<operand;
      cout<<endl;

      string reg_code = registerCode(tokens[1]); 
      code.append("F");//RD RS 
      code.append(reg_code);//RD RS 
      code.append("01"); //UP AM
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=3;
    }
    else if(regex_search(operand, match, jmpRegIndRegex)){
      cout<<"reg indirektno"<<endl;
      cout<<operand;
      cout<<endl;

      string reg_code = registerCode(tokens[1]); 
      code.append("F"); 
      code.append(reg_code); //RS
      code.append("0");//UP
      code.append("2"); //AM
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=3;


    }
    else if(regex_search(operand, match, jmpRegIndDispRegex)){
      cout<<"reg indirektno sa pomerajem"<<endl;
      cout<<operand;
      cout<<endl;

      string reg_code = registerCode(tokens[1]); 
      code.append("F"); 
      code.append(reg_code); //RS
      code.append("0");//UP
      code.append("3"); //AM

      regex symbRgx("^[a-zA-Z][a-zA-Z0-9_]*$"); 
      //check if operand is symbol
      if(regex_search(tokens[2], match, symbRgx)){
        //operand is symbol
        cout<<"operand is symbol"<<endl; 
        //make relocation entry 
        string value = processSymbol(tokens[2], locationCounter+3);
        code.append(value); 
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }
      else{
        //operand is literal
        cout<<"operand is literal"<<endl; 
        operand = literalToHex(tokens[2]); 
        cout<<"operand: "<<operand<<endl;
        for(int i=operand.length(); i<4; i++){
          code.append("0"); 
        }
        for(int i=0; i<operand.length(); i++){
          code+=operand[i]; 
        }
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }
    }
    else if(regex_search(operand, match, jmpMemDirRegex)){
      cout<<"mem direktno"<<endl;
      cout<<operand;
      cout<<endl;

      code.append("F0");//RD RS 
      code.append("04"); //UP AM

      regex symbRgx("^[a-zA-Z][a-zA-Z0-9_]*$");
      operand=operand.substr(1,operand.length());
      //check if operand is symbol
      if(regex_search(operand, match, symbRgx)){
        //operand is symbol
        cout<<"operand is symbol"<<endl; 
        //make relocation entry 
        string value = processSymbol(operand, locationCounter+3);
        code.append(value); 
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      } 
      else{
        //operand is literal
        cout<<"operand is literal"<<endl; 
        operand = literalToHex(operand); 
        cout<<"operand: "<<operand<<endl;
        for(int i=operand.length(); i<4; i++){
          code+="0"; 
        }
        for(int i=0; i<operand.length(); i++){
          code+=operand[i]; 
        }
        cout<<"code"<<endl;
        cout<<code<<endl; 
        for(int i=0; i<code.length(); i++){
          sectionTable[currentSectionName].code.push_back(code[i]); 
        }
        locationCounter+=5;

      }

    }
    else if(regex_search(operand, match, jmpPcRelRegex)){
      cout<<"pc relativno"<<endl;
      cout<<operand;
      cout<<endl;
      code.append("F"); //RD
      code.append("7"); //RS is pc reg 
      code.append("05"); //UP AM 
      operand = operand.substr(1, operand.length()); 

      string value = processSymbolForRelocation(operand, locationCounter);
      code.append(value); 
      cout<<"code"<<endl;
      cout<<code<<endl; 
      for(int i=0; i<code.length(); i++){
        sectionTable[currentSectionName].code.push_back(code[i]); 
      }
      locationCounter+=5;
    }


  }
}

string Assembler::literalToHex(string token){
  int lit = stoi(token); 
  //literal must be withing range of 2 bytes 
  if(lit>WORD_MAX || lit<WORD_MIN){
    string msg ="Literal out of range! Error at line: "+ to_string(currLineNum);
    throw BadSynataxException(msg);
  }
  
  //TODO add hex regex 
  regex hexRegex("0x[0-9A-Fa-f]+"); 
  smatch sm; 
  if(!regex_search(token, sm, hexRegex)){
    //dec literal 
    token = decToHex(stoi(token)); 
  }
  // cout<<"Token is: "<<token<<endl; 
  regex hexPrefix("0x"); 
  token=regex_replace(token, hexPrefix, "");

  for (int i=0; i<token.length(); i++){
    token[i]=toupper(token[i]); 
  }
  return token; 
}

string Assembler::binToHex(string binary_str){
  bitset<8> set(binary_str);  
  stringstream res;
  res << hex << uppercase << set.to_ulong();
  return res.str();
}

string Assembler::binToHex16bit(string bin){
  bitset<16> set(bin);  
  stringstream res;
  res << hex << uppercase << set.to_ulong();
  return res.str();
}

string Assembler::decimalToHex(int decimal) {
    std::bitset<16> binary(decimal);
    std::string hex = std::bitset<16>(binary).to_string();
    return hex;
}


string Assembler::processSymbol(string token, int lc){
  string symbValue; 
  //check if symbol is not in symbol table 
  if(symbolTable.find(token)==symbolTable.end()){
    //add symbol to symbol table 
    symbolTable[token].isDefined=false; 
    symbolTable[token].isGlobal=false; 
    symbolTable[token].sectionNum=currentSectionNumber; //no section 
    symbolTable[token].size=-1;
    symbolTable[token].symbolId=symbolId;
    symbolTable[token].symbolName=token;
    symbolTable[token].value=0;
    symbolId++;
    cout<<"symbol "<<token<<" added to the symbol table"<<endl; 

    //add symbol to symbol use entry 
    //make symbol use entry 
    symbolUseEntry symbUse; 
    symbUse.address = lc; 
    symbUse.section = currentSectionNumber; 
    symbUse.type = 0; 
    symbolTable[token].useVector.push_back(symbUse); 

    //TODO make relocation entry 
    addRelocation(lc, 0, symbolTable[token].symbolId, 0, currentSectionName); 
    string value="0000"; 
    return value; 
  }
  else{
    //symbol is in symbol table 
    if(symbolTable[token].isGlobal==false){
      //symbol is local
      //add symbol to symbol use entry 
      //make symbol use entry 
      symbolUseEntry symbUse; 
      symbUse.address = lc; 
      symbUse.section = currentSectionNumber; 
      symbUse.type = 0; 
      symbolTable[token].useVector.push_back(symbUse); 

      //TODO make relocation entry 
      addRelocation(lc, 0, symbolTable[token].symbolId, 0, currentSectionName); 
    
      string symbValue = literalToHex(to_string(symbolTable[token].value)); 
      string value;
      for(int i=symbValue.length(); i<4; i++){
        //add leading zeros
        value+="0";
      }
      for(int i=0; i<symbValue.length(); i++){
        value+=symbValue[i]; 
      }
      cout<<"value of symbol "+token+" is "+value<<endl; 
      return value; 

    }
    else{
      //symbol is global
      symbolUseEntry symbUse; 
      symbUse.address = lc; 
      symbUse.section = currentSectionNumber; 
      symbUse.type = 0; 
      symbolTable[token].useVector.push_back(symbUse); 

      //TODO make relocation entry 
      addRelocation(lc, 0, symbolTable[token].symbolId, 0, currentSectionName); 
      string value="0000";
      return value;  
    }

  }
}

string Assembler::processSymbolForRelocation(string token, int lc){
    string symbValue; 
  //check if symbol is not in symbol table 
  if(symbolTable.find(token)==symbolTable.end()){
    //add symbol to symbol table 
    symbolTable[token].isDefined=false; 
    symbolTable[token].isGlobal=false; 
    symbolTable[token].sectionNum=currentSectionNumber; //no section 
    symbolTable[token].size=-1;
    symbolTable[token].symbolId=symbolId;
    symbolTable[token].symbolName=token;
    symbolTable[token].value=0;
    symbolId++;
    cout<<"symbol "<<token<<" added to the symbol table"<<endl; 

    //add symbol to symbol use entry 
    //make symbol use entry 
    symbolUseEntry symbUse; 
    symbUse.address = lc+3; 
    symbUse.section = currentSectionNumber; 
    symbUse.type = 1; 
    symbolTable[token].useVector.push_back(symbUse); 

    //TODO make relocation entry 
    addRelocation(lc+3, 1, symbolTable[token].symbolId, 0, currentSectionName); 

    string value="0000";
    return value; 
  }
  else{
    //symbol is in symbol table 
    if(symbolTable[token].isGlobal==false){
      //symbol is local
      //add symbol to symbol use entry 
      //make symbol use entry 
      symbolUseEntry symbUse; 
      symbUse.address = lc+3; 
      symbUse.section = currentSectionNumber; 
      symbUse.type = 1; 
      symbolTable[token].useVector.push_back(symbUse); 

      //TODO make relocation entry 
      addRelocation(lc+3, 1, symbolTable[token].symbolId, 0, currentSectionName); 
    
      int val = symbolTable[token].value-2; 
      symbValue= decimalToHex(val); 
      cout<<"bin value: "+symbValue<<endl; 
      symbValue=binToHex16bit(symbValue); 
      cout<<"hex value: "+symbValue<<endl; 
      string value; 

      for(int i=symbValue.length(); i<4; i++){
        //add leading zeros
        value+="0";
      }
      for(int i=0; i<symbValue.length(); i++){
        value+=symbValue[i]; 
      }

      //make it little endian - rotate bytes in symbValue
      string tmp=value.substr(0,2);
      value = value.substr(2,2); 
      value.append(tmp); 


      cout<<"value of little endian symbol "+token+" is "+value<<endl; 
      return value; 

    }
    else{
      //symbol is global
      symbolUseEntry symbUse; 
      symbUse.address = lc+3; 
      symbUse.section = currentSectionNumber; 
      symbUse.type = 1; 
      symbolTable[token].useVector.push_back(symbUse); 

      //TODO make relocation entry 
      addRelocation(lc+3, 1, symbolTable[token].symbolId, 0, currentSectionName); 
      
      string value="FEFF";//it's -2 in twos complement but in little endian format 
      return value;  
    }

  }
}

void Assembler::backpatch(){
  for(auto& s:symbolTable){
    for(auto& symbUse:s.second.useVector){
      
    }
  }
} 









