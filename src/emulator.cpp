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
        std::cout<<"Lepo zadati argumenti \n"; 
        return true; 
    }

    
    throw BadCmdArgsException(); 
    return true; 

}
