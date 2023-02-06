#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "exception.hpp"

using namespace std; 

class Emulator{
private:
    string inputFile; 
    string currentLine; 
    string globalCode; 
public:
    Emulator(){}
    bool checkCmdArguments(int argc, char* argv[]); 
    void openParseFile(); 

    //helper functions 
    vector<string> tokenizeLine(string line, string delimiters); 
    void printCode(string input); 


    

};