
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
  symbolUseEntry(){
    address=0; type=0; 
  }

};

struct symbolTableEntry{
  string symbolName; 
  int value; 
  int size; 
  bool isGlobal; 
  int symbolId;
  int sectionNum; 
  bool isDefined;
  vector<symbolUseEntry> useVector; 
  symbolTableEntry(){
    symbolName=""; value=0; size=0; isGlobal=false; symbolId=0; sectionNum=0;
    isDefined=false;
  }
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
  static int sectionNumber;
  static long locationCounter;  
  static int currentSectionNumber; 

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
  bool checkIfLabel(string currLine);
  bool checkIfGlobalDirective(string currLine); 
  bool checkIfExternDirective(string currLine); 
  bool checkIfSectionDirective(string currLine); 
  bool checkIfWordDirective(string currLine); 
  bool checkIfSkipDirective(string currLine); 
  bool checkIfEndDirective(string currLine); 

  void processLabel(string currLine); 
  void processGlobalDirective(string currLine); 
  void processExternDirective(string currLine); 
  void processSectionDirective(string currLine); 
  void processWordDirective(string currLine); 
  void processSkipDirective(string currLine);
  void processEndDirective(string currLine); 



};