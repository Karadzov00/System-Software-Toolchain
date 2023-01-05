
#include <iostream>
#include <regex>
#include "../inc/assembler.hpp"


int main(int argc, char* argv[]){

  std::cout <<"Entered main! \n"; 
  Assembler::checkCmdArguments(argc, argv);
  
  return 0; 
}