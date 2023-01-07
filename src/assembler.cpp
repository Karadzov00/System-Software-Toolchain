#include "../inc/assembler.hpp"
#include "../inc/regex.hpp"

string Assembler::optionArg="";
string Assembler::outputFile="";
string Assembler::inputFile =""; 
string Assembler::currentLine="";
string Assembler::cleanCurrentLine="";

using namespace std; 


bool Assembler::checkCmdArguments(int argc, char* argv[]){
  
  for (int i = 0; i < argc; i++){
    //not enough arguments 
    if(!argv[i])throw BadCmdArgsException(); 
  }
  optionArg = argv[1]; 
  outputFile = argv[2]; 
  inputFile = argv[3]; 

  std::cout<<inputFile<<"\n"<<outputFile<<"\n"; 

  if(std::regex_match(optionArg, optionArgRegex) 
  && std::regex_match(inputFile, inputFileRegex)
  && std::regex_match(outputFile, outputFileRegex)){
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

  while(getline(inFile, currentLine)){
    smatch regexMatch;
    cleanCurrentLine = currentLine; 
    if(regex_search(currentLine, regexMatch, commentRegex)){
      cout<<cleanCurrentLine<<"\n";//before replacement
      //removing all comments 
      regex_replace(cleanCurrentLine, commentRegex, ""); 
      cout<<cleanCurrentLine<<"\n";//after replacement 
    
    }
  }


}