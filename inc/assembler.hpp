
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
  static string outputFile;
  static string inputFile;
  static string currentLine; 
  static string cleanCurrentLine; 


public:
  Assembler(); 
  static bool checkCmdArguments(int argc, char* argv[]); 
  static void openParseFile(); 
  void cleanInputFile(); 

};