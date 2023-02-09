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
    memset(memory, '0', sizeof(memory));
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

void Emulator::errorRoutine(){
    //write psw and pc to stack 
    registers[sp]-=2; 
    writeTwoBytesLittleEndian(registers[sp], registers[psw]); 
    registers[sp]-=2; 
    writeTwoBytesLittleEndian(registers[sp], registers[pc]); 
    
    //write address from IVT entry 1 to PC 
    int irAddress = hexToDecUnsigned(readTwoBytesLittleEndian(0));  
    registers[pc]=irAddress;
    cout<<"ERROR ROUTINE"<<endl; 
    
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

short Emulator::hexToDecSigned(string hex){
    unsigned short x;   
    std::stringstream ss;
    ss << std::hex << hex;
    ss >> x;
    // output it as a signed type
    return static_cast<short>(x);

}

string Emulator::decimalToHex(int decimal) {
    std::stringstream ss;
    ss<< std::hex << decimal; // int decimal_value
    std::string res ( ss.str() );
    return res; 
}

void Emulator::printInstruction(){
    cout<<"instruction number:"+instruction.code<<endl; 
    cout<<"reg dest:"+instruction.regDest<<endl; 
    cout<<"reg source:"+instruction.regSource<<endl; 
    cout<<"update mode:"+instruction.updateMode<<endl; 
    cout<<"addr mode:"+instruction.addrMode<<endl; 

}

void Emulator::printRegOperand(){
    cout<<"regD: "<<to_string(instruction.regDest)<<endl; 
    cout<<"regS: "<<to_string(instruction.regSource)<<endl; 
    cout<<"operand: "<<to_string(instruction.operand)<<endl; 
}


void Emulator::printRegisters(){
    cout<<endl; 
    cout<<"registers:"<<endl;
    cout<<"pc: "+decimalToHex(registers[pc])<<endl; 
    cout<<"sp: "+decimalToHex(registers[sp])<<endl; 
    cout<<"psw: "+decimalToHex(registers[psw])<<endl; 
    cout<<"r0: "+decimalToHex(registers[r0])<<endl; 
    cout<<"r1: "+decimalToHex(registers[r1])<<endl; 
    cout<<"r2: "+decimalToHex(registers[r2])<<endl; 
    cout<<"r3: "+decimalToHex(registers[r3])<<endl; 
    cout<<"r4: "+decimalToHex(registers[r4])<<endl; 
    cout<<"r5: "+decimalToHex(registers[r5])<<endl;
    cout<<"sp+2: -> "+readOneByte(registers[sp]+3)<<endl;  
    cout<<"sp+2: -> "+readOneByte(registers[sp]+2)<<endl;  
    cout<<"sp+1: -> "+readOneByte(registers[sp]+1)<<endl;  
    cout<<"sp: -> "+readOneByte(registers[sp])<<endl;  
    cout<<endl; 

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

void Emulator::writeTwoBytes(int address, short payload){
    address*=2; 
    std::stringstream ss;
    ss<< std::hex << payload; // int decimal_value
    std::string res ( ss.str() );
    for(int i=0; i<res.length();i++){
        res[i]=toupper(res[i]); 
    }
    for(int i=res.length();i<4;i++){
        res.insert(0, "0"); 
    }
    cout<<"payload for memory: "<<res<<endl; 
    cout<<"address: "+to_string(address)<<endl; 

    for(int i=0; i<4; i++){
        memory[address+i]=res[i];
    }
}

void Emulator::writeTwoBytesLittleEndian(int address, short payload){
    cout<<"address: "+decimalToHex(address)<<endl; 
    address*=2; 
    std::stringstream ss;
    ss<< std::hex << payload; // int decimal_value
    std::string res ( ss.str() );
    for(int i=0; i<res.length();i++){
        res[i]=toupper(res[i]); 
    }
    for(int i=res.length();i<4;i++){
        res.insert(0, "0"); 
    }
    cout<<"payload for memory: "<<res<<endl; 

    string tmp = res.substr(2,2);
    tmp.append(res.substr(0,2)); 
    res=tmp; 
    cout<<"little endian :"+res<<endl;  


    for(int i=0; i<4; i++){
        memory[address+i]=res[i];
    }
}

void Emulator::emulate(){
    resetProcessor(); 
    int cnt=0; 
    while(true){
        fetchInstrucionAndOperands(); 
        if(haltFlag){
            cout<<"Emulation finished"<<endl; 
            break; 
        }
        
        //for testing only
        if(cnt==300)
            break; //for testing only 
        cnt++;     
    }
    // cout<<"Emulation finished"<<endl; 
}

void Emulator::fetchInstrucionAndOperands(){
    // string istrCode = readOneByte(registers[pc]); 
    // cout<<"first byte is "+istrCode<<endl; 
    string opCode = readOneByte(registers[pc]); 
    cout<<endl; 
    printRegisters(); 
    cout<<"instruction: "+opCode<<endl; 
    cout<<"instruction address: "+decimalToHex(registers[pc])<<endl; 
    instruction.code = findInstruction(opCode); 

    switch(instruction.code){
        case HALT:{
            haltFlag=true; 
            break; 
        }
        case INTERR:{   
            fetchOperands2Bytes(); 
            executeINT(); 
            break;  
        }
        case IRET:{
            executeIRET(); 
            break;
        }
        case CALL:{
            fetchOperandsLdrJumps(); 
            executeCALL(); 
            break; 
        }
        case RET:{
            executeRET(); 
            break;
        }
        case JMP:{
            cout<<"jmp instruction"<<endl; 
            fetchOperandsLdrJumps(); 
            executeJMP(); 
            break;
        }
        case JEQ:{
            fetchOperandsLdrJumps(); 
            executeJEQ(); 
            break; 
        }
        case JNE:{
            fetchOperandsLdrJumps(); 
            executeJNE(); 
            break;
        }
        case JGT:{
            fetchOperandsLdrJumps(); 
            executeJGT(); 
            break;
        }
        case XCHG:{
            fetchOperands2Bytes();
            executeXCHG(); 

            break; 
        }
        case ADD:{
            fetchOperands2Bytes();
            executeADD();
            break;
        }
        case SUB:{
            fetchOperands2Bytes();
            executeSUB(); 
            break;
        }
        case MUL:{
            fetchOperands2Bytes();
            executeMUL(); 
            break; 
        }
        case DIV:{
            fetchOperands2Bytes();
            executeDIV();
            break;
        }
        case CMP:{
            fetchOperands2Bytes();
            executeCMP();
            break;
        }
        case LOGICNOT:{
            fetchOperands2Bytes();
            executeNOT(); 
            break; 
        }
        case LOGICAND:{
            fetchOperands2Bytes();
            executeAND(); 
            break;
        }
        case LOGICOR:{
            fetchOperands2Bytes();
            executeOR();
            break;
        }
        case LOGICXOR:{
            fetchOperands2Bytes();
            executeXOR();
            break;
        }
        case LOGICTEST:{
            fetchOperands2Bytes();
            executeTEST(); 
            break;
        }
        case SHL:{
            fetchOperands2Bytes();
            executeSHL(); 
            break; 
        }
        case SHR:{
            fetchOperands2Bytes();
            executeSHR();
            break;
        }
        case LDR:{
            fetchOperandsLdrJumps(); 
            executeLDR();
            break;
        }
        case STR:{
            fetchOperandsLdrJumps(); 
            executeSTR(); 
            break; 
        }
        case ERRCODE:{
            //jump to interrupt routine 
            errorRoutine(); 
            break; 
        }
        default:
            errorRoutine();
            break; 
    }
// map<string, instrCode> instructionMap={{"00", HALT}, {"10", INTERR}, {"20", IRET},
//  {"30", CALL}, {"40", RET}, {"50", JMP}, {"51", JEQ}, {"52", JNE}, {"53", JGT}, 
// {"60", XCHG}, {"70", ADD}, {"71", SUB}, {"72", MUL}, {"73", DIV}, {"74", CMP},
//  {"80", LOGICNOT}, {"81", LOGICAND}, {"82", LOGICOR}, {"83", LOGICXOR},
// {"84", LOGICTEST}, {"90", SHL}, {"91", SHR}, {"A0", LDR}, {"B0", STR}};
}

void Emulator::fetchOperandsLdrJumps(){
    registers[pc]++; 
    string regDS = readOneByte(registers[pc]); 
    // cout<<"regs: "+regDS<<endl; 
    string d;
    d.push_back(regDS[0]); 
    string s;
    s.push_back(regDS[1]); 
    char regD = hexToDecUnsigned(d); 
    char regS = hexToDecUnsigned(s);
    cout<<"register dest: "+to_string(regD)<<endl;
    cout<<"register source: "+to_string(regS)<<endl;
    instruction.regDest=regD;
    instruction.regSource=regS;
    
    registers[pc]++; 
    string addrMode = readOneByte(registers[pc]); 
    instruction.updateMode=findUpdateType(addrMode[0]); 
    if(instruction.updateMode==ERRUPD){
        //incorrect update mode
        //jump to interrupt routine 
        errorRoutine(); 
    }
    instruction.addrMode=findAddressing(addrMode[1]);
    cout<<"addr mode: "+to_string(instruction.addrMode)<<endl; 
    cout<<"update mode: "+to_string(instruction.updateMode)<<endl; 
    unsigned short oldPC = registers[pc]; 
    switch(instruction.addrMode){
        case IMM:{
            //needs payload 
            cout<<"immediate"<<endl; 
            registers[pc]+=3;//to point at next instruction  
            oldPC++; 
            int dhdl =hexToDecSigned(readTwoBytes(oldPC));
            instruction.operand=dhdl; 
            cout<<"dhdl: "+decimalToHex(dhdl)<<endl;  
            cout<<"pc: "+decimalToHex(registers[pc])<<endl; 

            // short x = hexToDecSigned("FF79");
            // cout<<"negative value "+to_string(x)<<endl; 
            // cout<<"pc + x :"+to_string(registers[pc]+x)<<endl; 

        break; 
        }
        case REGDIR:{
            cout<<"reg dir"<<endl; 

            registers[pc]++;//to point at next instruction  
            instruction.operand = registers[instruction.regSource]; 
            
        break;
        }
        case REGDIRDISP:{
            cout<<"reg dir disp"<<endl; 

            //check if pcrel for jumps
            if(instruction.regSource==pc){
                //payload is little endian 
                registers[pc]+=3; 
                oldPC++; 
                //watch out for second complement 
                int dhdl =hexToDecSigned(readTwoBytesLittleEndian(oldPC));
                //check this! is pc value good? 
                instruction.operand=registers[pc]+dhdl; 

            }
            else{
                registers[pc]+=3; 
                oldPC++; 
                int dhdl = hexToDecSigned(readTwoBytes(oldPC));
                instruction.operand=registers[instruction.regSource]+dhdl; 
                
            }

        break;
        }
        case REGIN:{
            cout<<"reg ind"<<endl; 

            //before operand address is formed 
            if(instruction.updateMode==PREDECREMENT){
                registers[instruction.regSource]-=2; 
            }
            else if(instruction.updateMode==PREINCREMENT){
                registers[instruction.regSource]+=2; 
            }

            //main logic for forming operand
            registers[pc]++;//to point at next instruction  
            int memVal = hexToDecSigned(readTwoBytes(registers[instruction.regSource]));
            instruction.operand=memVal;

            //after operand address is formed 
            if(instruction.updateMode==POSTDECREMENT){
                registers[instruction.regSource]-=2; 
            }
            else if(instruction.updateMode==POSTINCREMENT){
                registers[instruction.regSource]+=2; 
            } 
            cout<<"memVal: "+decimalToHex(instruction.operand)<<endl;  
        break;
        }
        case REGINDDISP:{
            cout<<"reg ind disp"<<endl; 

            //before operand address is formed 
            if(instruction.updateMode==PREDECREMENT){
                registers[instruction.regSource]-=2; 
            }
            else if(instruction.updateMode==PREINCREMENT){
                registers[instruction.regSource]+=2; 
            }

            //check if pcrel for ldr/str
            if(instruction.regSource==pc){
                //payload is little endian 
                registers[pc]+=3; 
                oldPC++; 
                //watch out for second complement 
                int dhdl =hexToDecSigned(readTwoBytesLittleEndian(oldPC));
                //pc + offset 
                int address = registers[pc]+dhdl; 
                instruction.operand=hexToDecSigned(readTwoBytes(address)); 
            }
            else{
                int dhdl = hexToDecSigned(readTwoBytes(registers[pc]+1));
                int address = registers[instruction.regSource]+dhdl; 
                instruction.operand=hexToDecSigned(readTwoBytes(address)); 
            }

            //after operand address is formed 
            if(instruction.updateMode==POSTDECREMENT){
                registers[instruction.regSource]-=2; 
            }
            else if(instruction.updateMode==POSTINCREMENT){
                registers[instruction.regSource]+=2; 
            } 

        break;
        }
        case MEM:{
            cout<<"memory addr"<<endl; 

            //payload is little endian 
            registers[pc]+=3; 
            oldPC++;
            //to read payload
            int dhdl =hexToDecSigned(readTwoBytes(oldPC));
            // cout<<"dhdl: "+to_string(dhdl)<<endl; 
            //read from payload address in memory 
            instruction.operand=hexToDecSigned(readTwoBytes(dhdl)); 

        break;
        }
        case ERRADDR:{
            errorRoutine(); 
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

void Emulator::fetchOperands2Bytes(){
    registers[pc]++; 
    string regDS = readOneByte(registers[pc]); 
    // cout<<"regs: "+regDS<<endl; 
    string d;
    d.push_back(regDS[0]); 
    string s;
    s.push_back(regDS[1]); 
    char regD = hexToDecUnsigned(d); 
    char regS = hexToDecUnsigned(s);
    cout<<"register dest: "+to_string(regD)<<endl;
    cout<<"register source: "+to_string(regS)<<endl;
    instruction.regDest=regD;
    instruction.regSource=regS;
    //to point at the following instruction 
    registers[pc]++; 

}



void Emulator::executeHALT(){

}
void Emulator::executeINT(){
    // for testing
    // writeTwoBytes(0, 10);
    // writeTwoBytes(0, 16);
    // writeTwoBytes(0, 153);
    // writeTwoBytes(0, -2);
    // writeTwoBytes(0, -135);

    // //for testing push pop
    // int stp=hexToDecUnsigned("FEFE"); 
    // cout<<"sp: "+to_string(stp)<<endl; 
    // writeTwoBytes(stp, 255);
    // cout<<"read from sp: "+readTwoBytes(stp)<<endl;  
    cout<<"int instruction"<<endl; 
    registers[sp]-=2; 
    //push psw to stack 
    writeTwoBytesLittleEndian(registers[sp], registers[psw]); 
    int address = (registers[instruction.regDest]%8)*2; 
    registers[pc]=hexToDecSigned(readTwoBytesLittleEndian(address)); 



}
void Emulator::executeIRET(){
    registers[psw]=hexToDecSigned(readTwoBytesLittleEndian(registers[sp])); 
    registers[sp]+=2; 
    registers[pc]=hexToDecSigned(readTwoBytesLittleEndian(registers[sp])); 
    registers[sp]+=2; 


}
void Emulator::executeCALL(){
    registers[sp]-=2; 
    writeTwoBytesLittleEndian(registers[sp], registers[psw]); 
    registers[pc]=instruction.operand; 
}
void Emulator::executeRET(){
    registers[pc]=hexToDecSigned(readTwoBytesLittleEndian(registers[sp])); 
    registers[sp]+=2;
}
void Emulator::executeJMP(){

    //TESTING INT INSTR
    // int stp = hexToDecUnsigned("FEFE");
    // stp--; 
    // writeTwoBytesLittleEndian(stp, registers[psw]); 
    // registers[r5]=0; 
    // int address = (registers[r5]%8)*2; 
    // cout<<"address: " + to_string(address)<<endl;
    // cout<<memory[8]<<memory[9]<<memory[10]<<memory[11]<<endl; 
    // cout<<readTwoBytes(0)<<endl; 
    // registers[pc]=hexToDecSigned(readTwoBytes(8)); 
    // cout<<"stack: " + readTwoBytesLittleEndian(stp)<<endl;
    // cout<<"pc: "+to_string(registers[pc])<<endl;
    printRegOperand();
    registers[pc]=instruction.operand; 
    cout<<"pc after jmp instruction: "+decimalToHex(registers[pc])<<endl; 
}
void Emulator::executeJEQ(){
    if(bitC()){
        registers[pc]=instruction.operand; 
    }
}
void Emulator::executeJNE(){
    if(!bitC()){
        registers[pc]=instruction.operand; 
    }
}
void Emulator::executeJGT(){
    //if not zero and if not negative means is >0
    if(!bitZ() && !bitN()){
        registers[pc]=instruction.operand; 
    }
}
void Emulator::executeXCHG(){
    unsigned short tmp = registers[instruction.regDest]; 
    registers[instruction.regDest] = registers[instruction.regSource];
    registers[instruction.regSource] = tmp;
}
void Emulator::executeADD(){
    registers[instruction.regDest] = registers[instruction.regDest] + registers[instruction.regSource];
}
void Emulator::executeSUB(){
    registers[instruction.regDest] = registers[instruction.regDest] - registers[instruction.regSource];
}
void Emulator::executeMUL(){
    registers[instruction.regDest] = registers[instruction.regDest] * registers[instruction.regSource];
}
void Emulator::executeDIV(){
    if(instruction.operand==0){
        //call interrupt routine
    }
    registers[instruction.regDest] = registers[instruction.regDest] * registers[instruction.regSource];
}
void Emulator::executeCMP(){
    short temp = registers[instruction.regDest] - registers[instruction.regSource];
    //TODO update psw flags 
    if(temp==0){
        setZ();
    }
    else{
        resetZ();
    }
    if(temp<0){
        setZ();
    }
    else{
        resetZ(); 
    }
    if(registers[instruction.regDest] > registers[instruction.regSource]){
        setC(); 
    }
    else{
        resetC(); 
    }
    // short max_value = 32767;
    // short min_value = -32767;
    if( (registers[instruction.regDest] < 0 && registers[instruction.regSource] > 0 && temp > 0)
         || (registers[instruction.regDest] > 0 && registers[instruction.regSource] < 0 && temp < 0) ){
        setO(); 
    }
    else{
        resetO(); 
    }

    
}
void Emulator::executeNOT(){
    registers[instruction.regDest] = !registers[instruction.regDest]; 
}
void Emulator::executeAND(){
    registers[instruction.regDest] &= registers[instruction.regSource]; 

}
void Emulator::executeOR(){
    registers[instruction.regDest] |= registers[instruction.regSource]; 

}
void Emulator::executeXOR(){
    registers[instruction.regDest] ^= registers[instruction.regSource]; 

}
void Emulator::executeTEST(){
    int temp = registers[instruction.regDest] & registers[instruction.regSource]; 
    //TODO update PSW 
    if(temp==0){
        setZ();
    }
    else{
        resetZ();
    }
    if(temp<0){
        setZ();
    }
    else{
        resetZ(); 
    }
}
void Emulator::executeSHL(){
    int tmp = registers[instruction.regDest]; 
    registers[instruction.regDest] <<= registers[instruction.regSource]; 
    //TODO update PSW 
    if(registers[instruction.regDest]==0){
        setZ();
    }
    else{
        resetZ();
    }
    if(registers[instruction.regDest]<0){
        setZ();
    }
    else{
        resetZ(); 
    }
    if(tmp>registers[instruction.regDest]){
        setC();
    }
    else{
        resetC(); 
    }

}
void Emulator::executeSHR(){
    //for carry bit 
    unsigned short regS = registers[instruction.regSource];
    std::bitset<16> bits_a(registers[instruction.regDest]);
    bits_a >>= regS;
    unsigned short a = (unsigned short)bits_a.to_ulong();
    if(bits_a[15]){
        //if 15th bit is 1  
        setC();
    }
    else{
        resetC(); 
    }

    registers[instruction.regDest] >>= registers[instruction.regSource]; 
    //TODO update PSW 
    if(registers[instruction.regDest]==0){
    setZ();
    }
    else{
        resetZ();
    }
    if(registers[instruction.regDest]<0){
        setZ();
    }
    else{
        resetZ(); 
    }

}
void Emulator::executeLDR(){
    cout<<"ldr"<<endl; 
    printRegOperand(); 
    registers[instruction.regDest] = instruction.operand; 
}
void Emulator::executeSTR(){
    //check
    cout<<"str"<<endl; 
    printRegOperand();
    //store data from dest register on address operand 
    if(instruction.regSource==sp){
        cout<<"push"<<endl;
        writeTwoBytesLittleEndian(registers[instruction.regSource], registers[instruction.regDest]); 

    }
    else{
        cout<<"push"<<endl;
        writeTwoBytesLittleEndian(instruction.operand, registers[instruction.regDest]); 

    }
}

int Emulator::bitZ(){
    int bit = registers[psw] & 0x1;
    return bit;  
}
int Emulator::bitO(){
    int bit = registers[psw] & 0x2;
    bit = bit >> 1; 
    return bit;  

}
int Emulator::bitC(){
    int bit = registers[psw] & 0x4;
    bit = bit >> 2; 
    return bit;
}
int Emulator::bitN(){
    int bit = registers[psw] & 0x8;
    bit = bit >> 3; 
    return bit;
}
int Emulator::bitI(){
    int bit = registers[psw] & 0x8000;
    bit = bit >> 15; 
    return bit;
}

void Emulator::setZ(){
    registers[psw] |= 0x1; 
}
void Emulator::setO(){
    registers[psw] |= 0x2; 

}
void Emulator::setC(){
    registers[psw] |= 0x4; 

}
void Emulator::setN(){
    registers[psw] |= 0x8; 

}
void Emulator::setI(){
    registers[psw] |= 0x8000; 
}
void Emulator::resetZ(){
    registers[psw] &= 0xFFFE; 
}
void Emulator::resetO(){
    registers[psw] &= 0xFFFD; 
}
void Emulator::resetC(){
    registers[psw] &= 0xFFFB; 
}
void Emulator::resetN(){
    registers[psw] &= 0xFFF7; 
}
void Emulator::resetI(){
    registers[psw] &= 0x7FFF; 
} 

