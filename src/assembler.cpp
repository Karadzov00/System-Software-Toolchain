#include "../inc/assembler.hpp"
#include "../inc/regex.hpp"

string Assembler::optionArg="";
string Assembler::cmdOutputFile="";
string Assembler::cmdInputFile =""; 
string Assembler::currentLine="";
string Assembler::cleanCurrentLine="";

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
    smatch regexMatch;
    if(regex_search(cleanCurrentLine, regexMatch, emptyLineRegex))
      continue; //if empty line skip iteration

    formatLine(); 



    cleanLines.push_back(cleanCurrentLine);  
    currLineNum++; 
  }


}

void Assembler::formatLine(){
    smatch regexMatch;
    cleanCurrentLine = currentLine; 
    if(regex_search(cleanCurrentLine, regexMatch, commentRegex)){
      cout<<"comments \n"; 
      cout<<cleanCurrentLine<<"\n";//before replacement
      //remove all comments 
      cleanCurrentLine=regex_replace(cleanCurrentLine, commentRegex, ""); 
      cout<<cleanCurrentLine<<"\n";//after replacement 
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