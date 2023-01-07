
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



public:
  static string cmdOutputFile;
  static string cmdInputFile;

  Assembler(string inputFile, string outputFile):inputFile(inputFile),
  outputFile(outputFile){}
  static bool checkCmdArguments(int argc, char* argv[]); 
  void openParseFile(); 
  void cleanInputFile(); 

};