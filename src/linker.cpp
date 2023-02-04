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
        

    cout<<outputFile<<endl;
    for(auto f:inputFiles){
        cout<<f<<endl; 
    } 


    throw BadCmdArgsException(); 
    return true; 

}
void Linker::openParseFile(){

}