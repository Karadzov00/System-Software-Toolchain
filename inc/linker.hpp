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
  int type; 
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

class Linker{
private:

    vector<string>inputFiles; 
    string outputFile; 
    string optionArg; 
    int currLineNum; 
    string currentLine; 
    map<string, int>sectionSizes; 
    map<string, int>sectionAdresses; 
    int allSectionsSize=0; 
    map<string, symbolTableEntry> symbolTable; 
    map<string, int>globalSymbolTable; //symbol -> global address




public:

    Linker(){}
    bool checkCmdArguments(int argc, char* argv[]); 
    void openParseFile(); 

    //helper functions 
    vector<string> tokenizeLine(string line, string delimiters); 
    void printSymbolTable(); 
    string findSectionName(int num); 

};