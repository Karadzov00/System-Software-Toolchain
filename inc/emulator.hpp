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

    unsigned short registers[9];//8 regs + psw
    enum REGISTERS{r0, r1, r2, r3, r4, r5, sp, pc, r8, psw}; 
    char memory[65535]={0}; 
    int cntMem=0; 
    bool haltFlag; 
    
public:
    Emulator(){}
    bool checkCmdArguments(int argc, char* argv[]); 
    void writeCodeToMemory(); 
    void resetProcessor(); 
    void emulate(); 

    //helper functions 
    vector<string> tokenizeLine(string line, string delimiters); 
    void printCode(string input); 

    //get bits 
    short getZ(); 
    short getO(); 
    short getC(); 
    short getN(); 



    

};