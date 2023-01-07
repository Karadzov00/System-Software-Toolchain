
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>


#include "exception.hpp"

using namespace std; 

class Assembler{

private:

  vector<string> cleanLines; 
  static string optionArg;
  string outputFile;
  string inputFile;

  static string currentLine; 
  static string cleanCurrentLine; 
  int currLineNum; 
  int instrDirNum; 



public:
  static string cmdOutputFile;
  static string cmdInputFile;

  Assembler(string inputFile, string outputFile):inputFile(inputFile),
  outputFile(outputFile){
    this->currLineNum=1;
    this->instrDirNum=0;  
  }
  static bool checkCmdArguments(int argc, char* argv[]); 
  void openParseFile(); 
  void formatLine(); 
  void cleanInputFile(); 
  void checkIfGlobalDirective(); 
  void checkIfExternDirective(); 
  void checkIfSectionDirective(); 
  void checkIfWordDirective(); 
  void checkIfSkipDirective(); 
  void checkIfEndDirective(); 



};