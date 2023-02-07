#include "../inc/emulator.hpp"
#include "../inc/regex.hpp"
#include <iomanip>
#include <bitset>
#include <algorithm>
#include <set>


map<string, instrCode> instructionMap={{"00", HALT}, {"10", INTERR}, {"20", IRET},
 {"30", CALL}, {"40", RET}, {"50", JMP}, {"51", JEQ}, {"52", JNE}, {"53", JGT}, 
{"60", XCHG}, {"70", ADD}, {"71", SUB}, {"72", MUL}, {"73", DIV}, {"74", CMP},
 {"80", LOGICNOT}, {"81", LOGICAND}, {"82", LOGICOR}, {"83", LOGICXOR},
{"84", LOGICTEST}, {"90", SHL}, {"91", SHR}, {"A0", LDR}, {"B0", STR}};

map<char, addressing> addressingMap={{'0', IMM}, {'1', REGDIR}, {'5', REGDIRDISP},
 {'2', REGIN}, {'3', REGINDDISP}, {'4', MEM}};
// enum addressing{IMM, REGDIR, REGDIRDISP, REGIN, REGINDDISP, MEM};

map<char, update> updateMap={{'0', NOUPDATE}, {'1', PREDECREMENT}, {'2', PREINCREMENT},
 {'3', POSTDECREMENT}, {'4', POSTINCREMENT}};
// enum update{NOUPDATE, PREDECREMENT, PREINCREMENT, POSTDECREMENT, POSTINCREMENT}; 

bool Emulator::checkCmdArguments(int argc, char* argv[]){
    for (int i = 0; i < argc; i++){
        //not enough arguments 
        if(!argv[i])throw BadCmdArgsException(); 
    }
    string firstArg(argv[1]); 
    regex hexFileRegex("^.+\\.hex$"); 
    smatch match; 
    if(regex_search(firstArg, match, hexFileRegex)){
        inputFile=firstArg; 
        std::cout<<"input file: "+inputFile<<endl; 
        std::cout<<"Lepo zadati argumenti "<<endl; 
        return true; 
    }


    throw BadCmdArgsException(); 
    return true; 

}

void Emulator::writeCodeToMemory(){
    //write hex code to memory 
    fstream inFile; 
    inFile.open(inputFile); 
    if(!inFile.is_open())throw FileNotOpenException();
    cout<<inputFile+" - File opened! \n";  
    regex numColonRegex("^.+:");
    vector<string>tokens;  
    cout<<"MEMORY"<<endl; 
    while(getline(inFile, currentLine)){
        cout<<currentLine<<endl; 
        currentLine=regex_replace(currentLine, numColonRegex, ""); 
        tokens = tokenizeLine(currentLine, " ");
        for(auto t:tokens){
            //writing each byte to memory 
            memory[cntMem]=t[0]; 
            memory[cntMem+1]=t[1]; 
            cntMem+=2; 
        }


    }
    string code(memory, sizeof(memory)/sizeof(char)); 
    // printCode(memory); //writing to memory works 
    
}

void Emulator::resetProcessor(){
    for(int i=0; i<6; i++){
        registers[i]=0; 
    }
    string initialAddr = readTwoBytesLittleEndian(0);
    
    int firstAddress = hexToDecUnsigned(initialAddr);  
    cout<<"initial address: "+initialAddr<<endl; 
    cout<<"dec value: "+to_string(firstAddress)<<endl; 
    string target = readTwoBytes(firstAddress); 
    cout<<"target address: "+target<<endl; 

    registers[pc]=firstAddress;
    registers[sp]=0; 
    registers[psw]=0x8000; //masked interrupts on bit 15 
    haltFlag=false; 
    


}

instrCode Emulator::findInstruction(string code){
    if(instructionMap.find(code)!=instructionMap.end()){
        //code found 
        return instructionMap[code];
    }
    else{
        //code not found 
        return ERRCODE; 
    }
} 

addressing Emulator::findAddressing(char code){
    if(addressingMap.find(code)!=addressingMap.end()){
        //code found 
        return addressingMap[code];
    }
    else{
        //code not found 
        return ERRADDR; 
    }
} 


update Emulator::findUpdateType(char code){
    if(addressingMap.find(code)!=addressingMap.end()){
        //code found 
        return updateMap[code];
    }
    else{
        //code not found 
        return ERRUPD; 
    }
}



vector<string> Emulator::tokenizeLine(string line, string delimiters){
    char *cstr = new char[line.length() + 1];
    strcpy(cstr, line.c_str());

    char *delim = new char[delimiters.length() + 1];
    strcpy(delim, delimiters.c_str());

    char str[] ="- This, a sample string.";
    char * pch;
    vector<string>tokens; 
    // printf ("Splitting string \"%s\" into tokens:\n",cstr);
    pch = strtok (cstr,delim);
    while (pch != NULL)
    {
        string token(pch); 
        tokens.push_back(token); 
        // cout<<token; 
        pch = strtok (NULL, delim);
    }
    return tokens; 
}

void Emulator::printCode(string input){
    std::string result;
    int cnt=1;
    int cntLine=1; 
    for(auto c: input){
      cout<<c; 
      if(cnt==2){
        cout<<" ";
        cnt=0; 
      }
      if(cntLine==20){
        cout<<endl;
        cntLine=0; 
      }
      cnt++; 
      cntLine++; 
    }
    cout<<endl; 
} 

string Emulator::binToHex16bit(string bin){
  bitset<16> set(bin);  
  stringstream res;
  res << hex << uppercase << set.to_ulong();
  return res.str();
}

string Emulator::decimalToBin16bit(int decimal) {
    std::bitset<16> binary(decimal);
    std::string hex = std::bitset<16>(binary).to_string();
    return hex;
}

int Emulator::hexToDecUnsigned(string hex){
    // hex.insert(0,"0x"); 
    // cout<<"hex " + hex<<endl; 
    unsigned int x = std::stoul(hex, nullptr, 16);
    return x; 
}   

int Emulator::hexToDecSigned(string hex){
    int x = std::stoul(hex, nullptr, 16);
    return x; 
}


//little endian 
string Emulator::readOneByte(int address){
    address*=2; 
    char c1 = memory[address]; 
    char c2 = memory[address+1]; 
    string ret;
    ret.push_back(c1); 
    ret.push_back(c2);  
    return ret; 
} 

//big endian 
string Emulator::readTwoBytes(int address){
    address*=2; 
    char c1 = memory[address]; 
    char c2 = memory[address+1]; 
    char c3 = memory[address+2]; 
    char c4 = memory[address+3]; 

    string ret;
    ret.push_back(c1); 
    ret.push_back(c2);  
    ret.push_back(c3);  
    ret.push_back(c4);  
    return ret; 
} 
string Emulator::readTwoBytesLittleEndian(int address){
    address*=2; 
    char c1 = memory[address]; 
    char c2 = memory[address+1]; 
    char c3 = memory[address+2]; 
    char c4 = memory[address+3]; 

    string ret;
    ret.push_back(c3);  
    ret.push_back(c4);  
    ret.push_back(c1); 
    ret.push_back(c2);  
    return ret;
}




void Emulator::emulate(){
    resetProcessor(); 
    while(true){
        fetchInstrucionAndOperands(); 
        break; //for testing only 
        if(haltFlag){
            break; 
        }

    }
}

void Emulator::fetchInstrucionAndOperands(){
    // string istrCode = readOneByte(registers[pc]); 
    // cout<<"first byte is "+istrCode<<endl; 
    string opCode = readOneByte(registers[pc]); 
    cout<<"first byte is "+opCode<<endl; 
    instruction.code = findInstruction(opCode); 

    switch(instruction.code){
        case HALT:{
            haltFlag=true; 
            break; 
        }
        case INTERR:{

            break;
        }
        case IRET:{

            break;
        }
        case CALL:{

            break; 
        }
        case RET:{

            break;
        }
        case JMP:{
            cout<<"jmp instruction"<<endl; 
            fetchOperands(); 
            //proveri ispravnost operanada 
            

            break;
        }
        case JEQ:
            break; 
        case JNE:
            break;
        case JGT:
            break;
        case XCHG:
            break; 
        case ADD:
            break;
        case SUB:
            break;
        case MUL:
            break; 
        case DIV:
            break;
        case CMP:
            break;
        case LOGICNOT:
            break; 
        case LOGICAND:
            break;
        case LOGICOR:
            break;
        case LOGICXOR:
            break;
        case LOGICTEST:
            break;
        case SHL:
            break; 
        case SHR:
            break;
        case LDR:
            break;
        case STR:
            break; 
        case ERRCODE:
            break; 
    }
// map<string, instrCode> instructionMap={{"00", HALT}, {"10", INTERR}, {"20", IRET},
//  {"30", CALL}, {"40", RET}, {"50", JMP}, {"51", JEQ}, {"52", JNE}, {"53", JGT}, 
// {"60", XCHG}, {"70", ADD}, {"71", SUB}, {"72", MUL}, {"73", DIV}, {"74", CMP},
//  {"80", LOGICNOT}, {"81", LOGICAND}, {"82", LOGICOR}, {"83", LOGICXOR},
// {"84", LOGICTEST}, {"90", SHL}, {"91", SHR}, {"A0", LDR}, {"B0", STR}};
}

void Emulator::fetchOperands(){
    registers[pc]++; 
    string regDS = readOneByte(registers[pc]); 
    cout<<"regs: "+regDS<<endl; 
    string d;
    d.push_back(regDS[0]); 
    string s;
    s.push_back(regDS[1]); 
    char regD = hexToDecUnsigned(d); 
    char regS = hexToDecUnsigned(s);
    cout<<to_string(regD)<<endl;
    cout<<to_string(regS)<<endl;
    instruction.regDest=regD;
    instruction.regSource=regS;
    
    registers[pc]++; 
    string addrMode = readOneByte(registers[pc]); 
    cout<<"addr mode: "+addrMode<<endl; 
    instruction.updateMode=findUpdateType(addrMode[0]); 
    instruction.addrMode=findAddressing(addrMode[1]);
    switch(instruction.addrMode){
        case IMM:{
            //needs payload 
            registers[pc]++; 
            int dhdl =hexToDecUnsigned(readTwoBytes(registers[pc]));
            instruction.operand=dhdl; 
            cout<<"dhdl: "+to_string(instruction.operand)<<endl;  

        break; 
        }
        case REGDIR:{
            instruction.operand = instruction.regSource; 
            
        break;
        }
        case REGDIRDISP:{
            //check if pcrel for jumps
            if(instruction.regSource==pc){
                //payload is little endian 
                registers[pc]++; 
                //watch out for second complement 
                int dhdl =hexToDecSigned(readTwoBytesLittleEndian(registers[pc]));
                //check this! is pc value good? 
                instruction.operand=registers[pc]+dhdl; 

            }
            else{
                int dhdl = hexToDecUnsigned(readTwoBytes(registers[pc]+1));
                instruction.operand=registers[instruction.regSource]+dhdl; 
                
            }

        break;
        }
        case REGIN:{
            int memVal = hexToDecUnsigned(readTwoBytes(registers[instruction.regSource]));
            instruction.operand=memVal; 
            cout<<"memVal: "+to_string(instruction.operand)<<endl;  
        break;
        }
        case REGINDDISP:{
            //check if pcrel for ldr/str
            if(instruction.regSource==pc){
                
            }
            else{
                int dhdl = hexToDecUnsigned(readTwoBytes(registers[pc]+1));
                int address = registers[instruction.regSource]+dhdl; 
                instruction.operand=hexToDecUnsigned(readTwoBytes(address)); 
            }

        break;
        }
        case MEM:{
            registers[pc]++; 
            //read payload
            int dhdl =hexToDecUnsigned(readTwoBytes(registers[pc]));
            cout<<"dhdl: "+to_string(dhdl)<<endl; 
            //read from payload address in memory 
            instruction.operand=hexToDecUnsigned(readTwoBytes(dhdl)); 

        break;
        }
        case ERRADDR:{

        break;
        }

    }
    // switch(instruction.updateMode){
    //     case NOUPDATE:{
    //         break;
    //     }
    //     case PREDECREMENT:{
    //         break;
    //     }
    //     case PREINCREMENT:{
    //         break;
    //     }
    //     case POSTDECREMENT:{
    //         break;
    //     }
    //     case POSTINCREMENT:{
    //         break;
    //     }
    //     case ERRUPD:{
    //         break; 
    //     }
    // }
}
