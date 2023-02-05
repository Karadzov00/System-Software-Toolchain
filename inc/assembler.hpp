
#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <map>


#include "exception.hpp"

using namespace std; 

struct symbolUseEntry{
  int address; //address of LC where symbol is used 
  int type; //is symbol used as absolute or relative addressing - 0 for symbol, 1 for pcrel, 2 for word (little endian)  
  int section; //symbol reference 
  symbolUseEntry(){
    address=0; type=0; section=0; 
  }

};

struct relocationEntry{
  int offset; 
  int type; //0 for symbol, 1 for pcrel, 2 for word (little endian)  
  int symbol; 
  int addend; 
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

struct sectionTableEntry{
  string sectionName; 
  int sectionId; 
  long size; 
  vector<char>code; 
  long startAdress; 
  vector<relocationEntry>sectionRelocations; 
};

class Assembler{

private:

  vector<string> cleanLines; 
  static string optionArg;
  string outputFile;
  string inputFile;
  bool endFlag=false;

  static string currentLine; 
  static string cleanCurrentLine; 
  int currLineNum; 
  int instrDirNum; 

  static int symbolId; 
  static int sectionNumber;
  static long locationCounter;  
  static int currentSectionNumber; 
  static string currentSectionName; 

  vector<char> code; 
  map<string, symbolTableEntry> symbolTable; 
  map<string, sectionTableEntry> sectionTable; 
  enum wordRange{WORD_MAX=65536, WORD_MIN=-32768}; 
  

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
  bool checkIfInstruction(string currLine); 
  
  void processLabel(string currLine); 
  void processGlobalDirective(string currLine); 
  void processExternDirective(string currLine); 
  void processSectionDirective(string currLine); 
  void processWordDirective(string currLine); 
  void processSkipDirective(string currLine);
  void processEndDirective(string currLine); 
  void processInstruction(string currLine); 
  void addRelocation(int offset, int type, int symbol, int addend, string currSectionName); 
  string processSymbol(string symbol, int lc); 
  string processSymbolForRelocation(string token, int lc); 
  void backpatch(); 

  //helper functions
  string decToHex(int dec);
  int hexToDec(string hex); 
  string binToHex(string bin); 
  string binToHex16bit(string bin); 
  string TwosComplement(int n);
  string findSectionName(); 
  string decToBin(string dec);
  string registerCode(string reg);
  string literalToHex(string token);
  string decimalToHex(int decimal);
  string findSymbolName(int sectionNum);
  void printSymbolTable(); 
  void printRelocations(); 
  void printCode(); 
  void printToFile(); 
  void printLinkerInput(); 
  void createUndefinedSection(string sectionName); 



};