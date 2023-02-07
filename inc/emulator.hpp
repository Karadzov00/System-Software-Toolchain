#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "exception.hpp"

using namespace std; 

struct Instruction{
    instrCode instrCode;
    char regDest; 
    char regSource; 
    update updateMode; 
    addressing addrMode; 
    int dataHigh; 
    int dataLow;  
}; 
enum instrCode{HALT, INTERR, IRET, CALL, RET, JUMPS, XCHG, ARITHMETIC,
                LOGIC, SHIFT, LDR, STR};
enum jumpType{JMP, JEQ, JNE, JGT}; 
enum arithmeticType{ADD, SUB, MUL, DIV, CMP};
enum logicType{LOGICNOT, LOGICAND, LOGICOR, LOGICXOR, LOGICTEST}; 
enum shiftType{SHL, SHR}; 
enum addressing{IMM, REGDIR, REGDIRDISP, REGIN, REGINDDISP, MEM};
enum update{NOUPDATE, PREDECREMENT, PREINCREMENT, POSTDECREMENT, POSTINCREMENT}; 

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
    Instruction instruction; 
    
public:
    Emulator(){}
    bool checkCmdArguments(int argc, char* argv[]); 
    void writeCodeToMemory(); 
    void resetProcessor(); 
    void emulate(); 
    void fetchInstrucionAndOperands(); 
    void executeInstrucion(); 
    void interrupt(); 

    //helper functions 
    vector<string> tokenizeLine(string line, string delimiters); 
    void printCode(string input); 
    string readOneByte(int address); 
    string readTwoBytes(int address);
    string readTwoBytesLittleEndian(int address);
    int hexToDecUnsigned(string hex);   
    int hexToDecSigned(string hex); 
    string binToHex16bit(string bin);
    string decimalToBin16bit(int decimal); 


    //get bits 
    short getZ(); 
    short getO(); 
    short getC(); 
    short getN(); 



    

};