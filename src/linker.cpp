#include "../inc/linker.hpp"
#include "../inc/regex.hpp"
#include <iomanip>
#include <bitset>


bool Linker::checkCmdArguments(int argc, char* argv[]){
    for (int i = 0; i < argc; i++){
        //not enough arguments 
        if(!argv[i])throw BadCmdArgsException(); 
    }

    string firstArg(argv[1]); 
    if(firstArg=="-hex"){
        optionArg = argv[2]; 
        outputFile = argv[3];
        for(int i=4; i<argc; i++){
            inputFiles.push_back(argv[i]); 
        }
    }
        
    regex inputRgx("^.*\\.o$");
    // cout<<outputFile<<endl;
    for(auto f:inputFiles){
        cout<<f<<endl; 
        if(!std::regex_match(f, inputRgx))
            throw BadCmdArgsException(); 

    } 

    regex outRgx("^.*\\.hex$");

    if(std::regex_match(optionArg, optionArgRegex) 
  && std::regex_match(outputFile, outRgx)){
    std::cout<<"Lepo zadati argumenti \n"; 
    return true; 
  }


    throw BadCmdArgsException(); 
    return true; 

}
void Linker::openParseFile(){
    for(auto inputFile:inputFiles){
        fstream inFile;
        inFile.open(inputFile); 
        if(!inFile.is_open())throw FileNotOpenException();
        cout<<inputFile+" - File opened! \n";  
        currLineNum=1; 

    }

}