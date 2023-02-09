
#include <iostream>
#include <regex>
#include "../inc/assembler.hpp"


int main(int argc, char* argv[]){

  // std::cout <<"Entered main! \n"; 
  try
  {
    Assembler::checkCmdArguments(argc, argv);
    Assembler assembler(Assembler::cmdInputFile,
    Assembler::cmdOutputFile); 
    assembler.openParseFile(); 

  }
  catch(const std::exception& e)
  {
    std::cout << e.what() << '\n';
  }
  
  
  return 0; 
}