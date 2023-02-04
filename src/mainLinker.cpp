#include <iostream>
#include <regex>
#include "../inc/linker.hpp"


int main(int argc, char* argv[]){

  std::cout <<"Entered main! \n"; 
  try
  {
    Linker linker; 
    linker.checkCmdArguments(argc, argv);
    linker.openParseFile(); 
   

  }
  catch(const std::exception& e)
  {
    std::cout << e.what() << '\n';
  }
  
  
  return 0; 
}