#include "../inc/assembler.hpp"
#include "../inc/regex.hpp"
#include <iomanip>

string Assembler::optionArg="";
string Assembler::cmdOutputFile="";
string Assembler::cmdInputFile =""; 
string Assembler::currentLine="";
string Assembler::cleanCurrentLine="";
int Assembler::symbolId=0;
int Assembler::sectionNumber=0; 
long Assembler::locationCounter=0; 
int Assembler::currentSectionNumber=0; 

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

    //throw instruction/directive not found 

  }
  cout<<"TABELA SIMBOLA:"<<endl; 
  // cout<<"name \t isGlobal \t id \t section \t size"<<endl;
  cout<< left<< setw(14)<<setfill(' ')<<"symbolName";
  cout<< left<< setw(14)<<setfill(' ')<<"isGlobal";
  cout<< left<< setw(14)<<setfill(' ')<<"symbolID";
  cout<< left<< setw(14)<<setfill(' ')<<"sectionNum";
  cout<< left<< setw(14)<<setfill(' ')<<"size";
  cout<< left<< setw(14)<<setfill(' ')<<"value";
  cout<<endl; 
  for (auto const& x : symbolTable){
    cout<< left<< setw(14)<<setfill(' ')<<x.second.symbolName;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.isGlobal;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.symbolId;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.sectionNum;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.size;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.value;
    cout<<endl; 

    // cout<<x.second.symbolName<<"\t"<<x.second.isGlobal<<"\t"<<x.second.symbolId<<"\t"<<x.second.sectionNum<<"\t"<<x.second.size<<endl;
  }
  cout<<"TABELA KORISCENJA:"<<endl; 
    cout<< left<< setw(14)<<setfill(' ')<<"symbol";
    cout<< left<< setw(14)<<setfill(' ')<<"address";
    cout<< left<< setw(14)<<setfill(' ')<<"section";
    cout<< left<< setw(14)<<setfill(' ')<<"type";
    cout<<endl; 

  for (auto const& x : symbolTable){
    for(auto u: x.second.useVector){
      // cout<<x.first<<": "<<u.address<<" "<<u.section<<" "<<u.type<<endl;
      cout<< left<< setw(14)<<setfill(' ')<<x.first;
      cout<< left<< setw(14)<<setfill(' ')<<u.address;
      cout<< left<< setw(14)<<setfill(' ')<<u.section;
      cout<< left<< setw(14)<<setfill(' ')<<u.type;
      cout<<endl; 

    }
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
    return true; 
  } 
  return false; 
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

      symbolUseEntry symbUse; 
      symbUse.address = locationCounter; 
      symbUse.section = currentSectionNumber; 
      symbUse.type = 0; 
      symbolTable[symbolName].useVector.push_back(symbUse); 
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

    symbolUseEntry symbUse; 
    symbUse.address = locationCounter; 
    symbUse.section = currentSectionNumber; 
    symbUse.type = 0; 
    symbolTable[symbolName].useVector.push_back(symbUse); 
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
      code.push_back('0'); //write 0 in code 
      cout<<"symbol "<<s<<" added to the symbol table"<<endl; 

      //make symbol use entry 
      symbolUseEntry symbUse; 
      symbUse.address = locationCounter; 
      symbUse.section = currentSectionNumber; 
      symbUse.type = 0; 
      symbolTable[s].useVector.push_back(symbUse); 

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

  for(int i=0; i<bytes; i++){
    code.push_back('0'); 
  }
  locationCounter+=bytes; 

}





