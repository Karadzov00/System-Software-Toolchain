#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <map>


#include "exception.hpp"

using namespace std; 

class Linker{
private:

    vector<string>inputFiles; 
    string outputFile; 
    string optionArg; 
    int currLineNum; 


public:

    Linker(){}
    bool checkCmdArguments(int argc, char* argv[]); 
    void openParseFile(); 

};