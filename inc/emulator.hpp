#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <map>

#include "exception.hpp"

using namespace std; 

enum instrCode{HALT, INTERR, IRET, CALL, RET, JMP, JEQ, JNE, JGT, XCHG, ADD, SUB, MUL,
 DIV, CMP, LOGICNOT, LOGICAND, LOGICOR, LOGICXOR, LOGICTEST, SHL, SHR, LDR, STR, ERRCODE};
// enum jumpType{JMP, JEQ, JNE, JGT}; 
// enum arithmeticType{ADD, SUB, MUL, DIV, CMP};
// enum logicType{LOGICNOT, LOGICAND, LOGICOR, LOGICXOR, LOGICTEST}; 
// enum shiftType{SHL, SHR}; 
enum addressing{IMM, REGDIR, REGIN, REGINDDISP, MEM, REGDIRDISP, ERRADDR};
enum update{NOUPDATE, PREDECREMENT, PREINCREMENT, POSTDECREMENT, POSTINCREMENT, ERRUPD}; 
struct Instruction{
    instrCode code;
    char regDest; 
    char regSource; 
    update updateMode; 
    addressing addrMode; 
    short operand;  
}; 



class Emulator{
private:
    string inputFile; 
    string currentLine; 
    string globalCode; 

    unsigned short registers[9];//8 regs + psw
    enum REGISTERS{r0, r1, r2, r3, r4, r5, sp, pc, r8, psw}; 
    char memory[131071] = {}; 
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
    void fetchOperands(); 
    void executeInstrucion(); 
    void interrupt(); 

    //helper functions 
    vector<string> tokenizeLine(string line, string delimiters); 
    void printCode(string input); 
    string readOneByte(int address); 
    string readTwoBytes(int address);
    void writeTwoBytes(int address, short payload);
    void writeTwoBytesLittleEndian(int address, short payload);
    string readTwoBytesLittleEndian(int address);
    int hexToDecUnsigned(string hex);   
    short hexToDecSigned(string hex); 
    string binToHex16bit(string bin);
    string decimalToBin16bit(int decimal); 
    instrCode findInstruction(string code); 
    addressing findAddressing(char code);
    update findUpdateType(char code); 
    //get bits 
    short getZ(); 
    short getO(); 
    short getC(); 
    short getN(); 

    //instructions 
    void executeHALT();
    void executeINT();
    void executeIRET();
    void executeCALL();
    void executeRET();
    void executeJMP();
    void executeJEQ();
    void executeJNE();
    void executeJGT();
    void executeXCHG();
    void executeADD();
    void executeSUB();
    void executeMUL();
    void executeDIV();
    void executeCMP();
    void executeNOT();
    void executeAND();
    void executeOR();
    void executeXOR();
    void executeTEST();
    void executeSHL();
    void executeSHR();
    void executeLDR();
    void executeSTR();



// enum instrCode{HALT, INTERR, IRET, CALL, RET, JMP, JEQ, JNE,
//  JGT, XCHG, ADD, SUB, MUL, DIV, CMP,
// LOGICNOT, LOGICAND, LOGICOR, LOGICXOR, LOGICTEST, SHL, SHR, LDR, STR, ERRCODE};

    

};