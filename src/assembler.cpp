#include "../inc/assembler.hpp"

bool Assembler::checkCmdArguments(int argc, char* argv[]){
  
  for (int i = 0; i < argc; i++){
    //not enough arguments 
    if(!argv[i])throw BadCmdArgsException(); 
  }
  std::string optionArg = argv[1]; 
  std::string outputFile = argv[2]; 
  std::string inputFile = argv[3]; 

  std::regex optionArgRegex("^-o$"); 
  std::regex outputFileRegex("^.*\\.o$"); 
  std::regex inputFileRegex("^.*\\.s$"); 

  std::cout<<inputFile<<"\n"<<outputFile<<"\n"; 

  if(std::regex_match(optionArg, optionArgRegex) 
  && std::regex_match(inputFile, inputFileRegex)
  && std::regex_match(outputFile, outputFileRegex)){
    std::cout<<"Lepo zadati argumenti \n"; 
    return true; 
  }
  
  throw BadCmdArgsException(); 
}