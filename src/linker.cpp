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

        while(getline(inFile, currentLine)){
            vector<string>tokens; 
            tokens=tokenizeLine(currentLine, ":"); 
                cout<<"TOKENS ARE: ";  
            for(auto t:tokens){
                cout<<t<<" "; 
            }
            cout<<endl; 

        }

    }

}

vector<string> Linker::tokenizeLine(string line, string delimiters){

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
