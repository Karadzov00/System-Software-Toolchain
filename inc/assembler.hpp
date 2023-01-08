
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>


#include "exception.hpp"

using namespace std; 

struct symbolUseEntry{
  int address; //address of LC where symbol is used 
  int type; //is symbol used as absolute ore relative addressing 

};

struct symbolTableEntry{
  string symbolName; 
  int value; 
  int size; 
  bool isGlobal; 
  int symbolId;
  int sectionNum; 
  vector<symbolUseEntry> useVector; 
};

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

  static int symbolId; 

  vector<char> code; 
  vector<symbolTableEntry> symbolTable; 
  
  
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

  void processGlobalDirective(); 
  void processExternDirective(); 
  void processSectionDirective(); 
  void processWordDirective(); 
  void processSkipDirective();
  void processEndDirective(); 



};