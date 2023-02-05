#include "../inc/linker.hpp"
#include "../inc/regex.hpp"
#include <iomanip>
#include <bitset>


int Linker::globalId=1; 

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
        partionedSections.clear(); 
        fileSections.clear(); //since it's local clear content 
        while(getline(inFile, currentLine)){
            if(currentLine=="section begin")break; 
            vector<string>tokens; 
            tokens=tokenizeLine(currentLine, ":"); 
                cout<<"TOKENS ARE: ";  
            for(auto t:tokens){
                cout<<t<<" "; 
            }
            cout<<endl; 
            string name = tokens[0];
            int isGlobal = stoi(tokens[1]);
            int symbolId = stoi(tokens[2]);
            int sectionNum = stoi(tokens[3]);
            int value = stoi(tokens[4]);
            int size = stoi(tokens[5]);
            cout<<"size is "<<size<<endl;
            
            //TODO detect double definition 
            if(sectionNum!=0){
                //if symbol is defined 
                symbolTable[name].isGlobal=isGlobal; 
                symbolTable[name].sectionNum=sectionNum; //no section 
                symbolTable[name].size=size;
                symbolTable[name].symbolId=globalId++;
                symbolTable[name].symbolName=name;
                symbolTable[name].value=value; 
                symbolTable[name].file=inputFile; 
                if(size==-1){
                    symbolTable[name].sectionName=fileSections[sectionNum];     
                }   
                else{
                    symbolTable[name].sectionName=name;  
                    fileSections[symbolId]=name;   
                } 

            }
            //TODO dodaj polje section name u globalnu tabelu simbola 

            if(size!=-1 && name!="UNDEFINED"){
                //it is a section 
                 //local mapping for each file 
                //check if it exists in hash map
                if(sectionSizes.find(name)==sectionSizes.end()){
                    cout<<"New section"<<endl; 
                    //not in hash map
                    sectionSizes[name]=size;  
                    cout<<"section: "+name<<", size:"+to_string(size)<<endl; 
                    allSectionsSize+=size; 
                    sections.push_back(name); 
                    fileSectionEntry entry; 
                    entry.name=name; 
                    entry.file=inputFile; 
                    entry.offset=0; //offset compared to other same name sections
                    entry.id=symbolId; 
                    partionedSections.push_back(entry); 

                }
                else{
                    //section already exists 
                    cout<<"Section already exists"<<endl; 
                    fileSectionEntry entry; 
                    entry.name=name; 
                    entry.file=inputFile; 
                    entry.id=symbolId; 
                    entry.offset=sectionSizes[name]; //offset compared to other same name sections
                    partionedSections.push_back(entry); 
                    cout<<"section: "+name<<", offset:"+to_string(sectionSizes[name])<<endl; 
                    sectionSizes[name]+=size;  
                    cout<<"section: "+name<<", new size:"+to_string(sectionSizes[name])<<endl; 
                    allSectionsSize+=size; 
                }
            }
        }
        sort(partionedSections.begin(), partionedSections.end(), [](fileSectionEntry a, fileSectionEntry b) {
            return a.id < b.id;
        });

        for(auto ps: partionedSections){
            globalSections.push_back(ps); 
        }

    }
    int lc=0; 
    //TOFIX iteriranje kroz mapu nije ovde fifo
    cout<<"SECTIONS FIFO"<<endl; 
    //global sections is vector so it is fifo 
    for(auto s:globalSections){
        cout<<"section "<<s.name<<" "<<s.file<<"; offset: "<<s.offset<<"; size:"<<sectionSizes[s.name]<<endl; 
        //if address is not yet calculated 
        if(sectionAdresses.find(s.name)==sectionAdresses.end()){
            sectionAdresses[s.name]=lc; 
            lc+=sectionSizes[s.name];
        }
    }

    cout<<endl; 


    printSymbolTable(); 
    for(auto s:sectionAdresses){
        cout<<"section: "+s.first<<", address:"+to_string(s.second)<<endl; 
    }
    cout<<endl; 

    for(auto s: symbolTable){
        int sectionOffset = findFileSectionOffset(s.second.file, s.second.sectionName); 
        if(sectionOffset!=-1){
            globalSymbolTable[s.first]=sectionOffset+s.second.value; 
        }
    }

    printGlobalSymbolTable(); 

}

int Linker::findFileSectionOffset(string file, string section){
    for(auto s: globalSections){
        if(file.compare(s.file)==0 && section.compare(s.name)==0){
            //found right section in right file 
            int offset = s.offset+sectionAdresses[s.name];  
            return offset; 
        }
    }
    return -1; 
} 


void Linker::printSymbolTable(){

  cout<<"TABELA SIMBOLA:"<<endl; 
  // cout<<"name \t isGlobal \t id \t section \t size"<<endl;
  cout<< left<< setw(14)<<setfill(' ')<<"symbolName";
  cout<< left<< setw(14)<<setfill(' ')<<"isGlobal";
  cout<< left<< setw(14)<<setfill(' ')<<"symbolID";
  cout<< left<< setw(14)<<setfill(' ')<<"sectionName";
  cout<< left<< setw(14)<<setfill(' ')<<"value";
  cout<< left<< setw(14)<<setfill(' ')<<"size";
  cout<<endl; 
  for (auto const& x : symbolTable){
    cout<< left<< setw(14)<<setfill(' ')<<x.second.symbolName;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.isGlobal;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.symbolId;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.sectionName;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.value;
    cout<< left<< setw(14)<<setfill(' ')<<x.second.size;
    cout<<endl; 

    // cout<<x.second.symbolName<<"\t"<<x.second.isGlobal<<"\t"<<x.second.symbolId<<"\t"<<x.second.sectionNum<<"\t"<<x.second.size<<endl;
  }
  cout<<endl; 

}

void Linker::printGlobalSymbolTable(){
  cout<<"GLOBALNA TABELA SIMBOLA:"<<endl; 
  cout<< left<< setw(14)<<setfill(' ')<<"symbolName";
  cout<< left<< setw(14)<<setfill(' ')<<"address";
  cout<<endl; 

  for (auto const& x : globalSymbolTable){
    cout<< left<< setw(14)<<setfill(' ')<<x.first;
    cout<< left<< setw(14)<<setfill(' ')<<x.second;
    cout<<endl; 
  }



}

string Linker::findSectionName(int num){
    for(auto s:symbolTable){
        if(s.second.symbolId==num)
            return s.second.symbolName; 
    }
    return ""; 
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
