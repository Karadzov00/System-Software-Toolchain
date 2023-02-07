#include "../inc/emulator.hpp"
#include "../inc/regex.hpp"
#include <iomanip>
#include <bitset>
#include <algorithm>
#include <set>



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
    cout<<"hex " + hex<<endl; 
    unsigned int x = std::stoul(hex, nullptr, 16);
    return x; 
}   

int Emulator::hexToDecSigned(string hex){
    int x = std::stoul(hex, nullptr, 16);
    return x; 
}


//little endian 
string Emulator::readOneByte(int address){
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
    string dummy = readTwoBytesLittleEndian(registers[pc]); 
    cout<<"first 2 bytes are "+dummy<<endl; 
    Instruction instruction; 
    


}
