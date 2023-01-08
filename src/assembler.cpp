#include "../inc/assembler.hpp"
#include "../inc/regex.hpp"

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
  for(symbolTableEntry entry:symbolTable){
    cout<<"naziv simbola: "<<entry.symbolName<<endl;
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
  for(symbolTableEntry entry: symbolTable){
    if(entry.symbolName.compare(symbolName)==0){
      //symbol exists in symbolTable
      cout<<"symbol found is symbol table"<<endl; 
      found=true; 
      entry.sectionNum=currentSectionNumber; 
      entry.value=locationCounter; 
    }
  }

  if(!found){
    //symbol doesn't exist in symbolTable
    symbolTableEntry symTabEntry;
    symTabEntry.isDefined=true;
    symTabEntry.isGlobal=false;
    symTabEntry.sectionNum=currentSectionNumber;
    symTabEntry.size=0; 
    symTabEntry.symbolId= symbolId; 
    symTabEntry.symbolName=symbolName;
    symTabEntry.value=locationCounter;  
    symbolId++; //increment static global id 
    symbolTable.push_back(symTabEntry);
    cout<<"simbol dodat u tabelu simbola"<<endl; 
  }

}

void Assembler::processGlobalDirective(string currLine){

}





