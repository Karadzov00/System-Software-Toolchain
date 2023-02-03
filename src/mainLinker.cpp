#include <iostream>
#include <regex>
#include "../inc/linker.hpp"


int main(int argc, char* argv[]){

  std::cout <<"Entered main! \n"; 
  try
  {
    Linker::checkCmdArguments(argc, argv);
    Linker linker(Linker::cmdInputFile,
    Linker::cmdOutputFile); 
    linker.openParseFile(); 

  }
  catch(const std::exception& e)
  {
    std::cout << e.what() << '\n';
  }
  
  
  return 0; 
}