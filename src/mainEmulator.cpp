#include <iostream>
#include <regex>
#include "../inc/emulator.hpp"


int main(int argc, char* argv[]){
    std::cout <<"Entered main emulator! \n"; 
    try
    {
        Emulator emulator; 
        emulator.checkCmdArguments(argc, argv); 
    

    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << '\n';
    }
    
    
    return 0; 
}