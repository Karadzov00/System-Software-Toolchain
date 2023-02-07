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
    while(getline(inFile, currentLine)){
        currentLine=regex_replace(currentLine, numColonRegex, ""); 
        cout<<currentLine<<endl; 
        tokens = tokenizeLine(currentLine, " ");
        for(auto t:tokens){
            //writing each byte to memory 
            memory[cntMem]=t[0]; 
            memory[cntMem+1]=t[1]; 
            cntMem+=2; 
        }


    }
    string code(memory, sizeof(memory)/sizeof(char)); 
    printCode(memory); 
    
}

void Emulator::resetProcessor(){
    for(int i=0; i<6; i++){
        registers[i]=0; 
    }
    registers[pc]=0;
    registers[sp]=0; 
    registers[psw]=0x8000; //masked interrupts on bit 15 
    haltFlag=false; 
    memory[65535]={0};


}

void Emulator::emulate(){
    while(true){
        
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


