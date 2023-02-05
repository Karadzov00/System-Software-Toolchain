#include "../inc/linker.hpp"
#include "../inc/regex.hpp"
#include <iomanip>
#include <bitset>
#include <algorithm>
#include <set>


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
            if(symbolTable.find(name)!=symbolTable.end() && size==-1 && sectionNum!=0 && isGlobal==1){
                //double definition
                string msg ="Error! Double definition of a symbol "+name;
                throw BadSynataxException(msg);
            }
            
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
            currLineNum++; 
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
    //global sections is vector so it is fifo 
    for(auto s:globalSections){
        // cout<<"section "<<s.name<<" "<<s.file<<"; offset: "<<s.offset<<"; size:"<<sectionSizes[s.name]<<endl; 
        //if address is not yet calculated 
        if(sectionAdresses.find(s.name)==sectionAdresses.end()){
            sectionAdresses[s.name]=lc; 
            lc+=sectionSizes[s.name];
        }
    }

    cout<<endl; 


    printSymbolTable(); 
    cout<<endl; 
    cout<<"SECTIONS FIFO"<<endl; 
    for(auto s:globalSections){
        cout<<"section "<<s.name<<" "<<s.file<<"; offset: "<<s.offset<<"; size:"<<sectionSizes[s.name]<<endl; 

    }
    cout<<endl; 

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
    processRelocations(); 

    for(auto s:globalSections){
        cout<<"section "<<s.name<<endl; 
        printCode(s.code);
        cout<<endl; 
    }




}

void Linker::processRelocations(){
    //process relocations 
    for(auto inputFile:inputFiles){
        fstream inFile;
        inFile.open(inputFile); 
        if(!inFile.is_open())throw FileNotOpenException();
        cout<<inputFile+" - File opened! \n";  
        currLineNum=1; 
        localSymbolTable.clear(); 
        //TODO generate local symbol table 
        while(getline(inFile, currentLine)){
            //while for symb table
            if(currentLine=="section begin")break; 


            vector<string>tokens; 
            tokens=tokenizeLine(currentLine, ":"); 
            
            string name = tokens[0];
            int isGlobal = stoi(tokens[1]);
            int symbolId = stoi(tokens[2]);
            int sectionNum = stoi(tokens[3]);
            int value = stoi(tokens[4]);
            int size = stoi(tokens[5]);
            

            localSymbolTable[name].symbolName=name;
            localSymbolTable[name].isGlobal=isGlobal;
            localSymbolTable[name].symbolId=symbolId;
            localSymbolTable[name].sectionNum=sectionNum;
            localSymbolTable[name].value=value;
            localSymbolTable[name].size=size;
            localSymbolTable[name].file=inputFile;

        }
        while(getline(inFile, currentLine)){
            //while for relocations 
            smatch match; 
            if(currentLine=="section begin"){
                getline(inFile, currentLine);
                currentSection=currentLine;
                getline(inFile, currentLine);
                // cout<<currentLine<<endl; 
                cout<<currentSection<<endl; 
                while(true){
                    if(currentLine=="code of section:"){
                        getline(inFile, currentLine);
                        string code=currentLine; 
                        cout<<code<<endl; 
                        code.erase(std::remove(code.begin(), code.end(), ':'), code.end());
                        cout<<code<<endl; 
                        writeCodeToGlobalSection(currentSection, inputFile, code); 
                        remakeCode(code, currentSection, inputFile); 
                        break; 
                    }
                    cout<<currentLine<<endl; 
                    relocationEntry reloc; 
                    vector<string>tokens=tokenizeLine(currentLine, ":");
                    for(auto t:tokens){
                        cout<<t+" "; 
                    }
                    cout<<endl; 
                    reloc.offset=stoi(tokens[0]);
                    reloc.type=stoi(tokens[1]);
                    reloc.symbol=stoi(tokens[2]);
                    reloc.addend=stoi(tokens[3]);
                    reloc.sectionName=currentSection; 
                    sectionRelocations.push_back(reloc); 
                    //make relocation table 
                    //read code into line
                    //change code with global symbol values 


                    getline(inFile, currentLine);
                } 

            }
        }

    }

}

void Linker::remakeCode(string code, string currentSection, string file){
    cout<<"relocation symbols:"<<endl; 
    for(auto r:sectionRelocations){
        if(r.sectionName==currentSection){
            int offset = r.offset; //offset in section
            int symbolID = r.symbol; 
            int addend = r.addend; 
            string symbol = findSymbolById(symbolID); 
            cout<<"symbol: "+symbol<<endl; 
            //check if symbol is in global symbol table 
            if(globalSymbolTable.find(symbol)==globalSymbolTable.end() && localSymbolTable[symbol].isGlobal==1){
                //symbol not defined
                string msg ="Error! Symbol not defined! Symbol: "+symbol;
                throw BadSynataxException(msg);
            }
            string globalValue; 
            if(symbolTable[symbol].isGlobal==1){
                globalValue = literalToHex(to_string(globalSymbolTable[symbol])); 
                cout<<"global value dec: "+to_string(globalSymbolTable[symbol])<<endl; 
            }
            else{
                //symbol is local 
                string section = findSymbolById(localSymbolTable[symbol].sectionNum);
                int offs = findFileSectionOffset(localSymbolTable[symbol].file, section);
                int val = offs + localSymbolTable[symbol].value;  
                globalValue=literalToHex(to_string(val)); 
                cout<<"global value dec: "+val<<endl; 
            }
            cout<<"global value hex: "+globalValue<<endl; 
            cout<<"offset: "+to_string(offset)<<endl; 
            cout<<"type: "+to_string(r.type)<<endl; 

            //absoulte addressing 
            if(r.type==0){
                //write new value in code 
                int position = offset*2; 
                for(int i=0; i<4; i++){
                    code[position+i]=globalValue[i]; 
                }
                cout<<"new code"<<endl; 
                printCode(code); 
                cout<<endl; 

                //add remade code to sections code  
                // sectionsCode[currentSection]=code; 
                writeCodeToGlobalSection(currentSection, file, code);
                

                //add remade code to global code for emulator 
                
            }
            //pcrel - little endian 
            else if(r.type==1){

            }
            //for word directive - little endian 
            else if(r.type==2){
                //write new value in code 
                int position = offset*2; 
                //make it little endian - rotate bytes in symbValue
                cout<<"big endian: "+globalValue<<endl; 
                string tmp=globalValue.substr(0,2);
                globalValue = globalValue.substr(2,2); 
                globalValue.append(tmp);
                cout<<"little endian: "+globalValue<<endl; 


                for(int i=0; i<4; i++){
                    code[position+i]=globalValue[i]; 
                }
                cout<<"new code"<<endl; 
                printCode(code); 
                cout<<endl; 

                //add remade code to sections code  
                // sectionsCode[currentSection]=code; 
                writeCodeToGlobalSection(currentSection, file, code);
                

                //add remade code to global code for emulator 

            }

        }
    }

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

void Linker::writeCodeToGlobalSection(string section, string file, string code){
    for(auto &s: globalSections){
        if(file.compare(s.file)==0 && section.compare(s.name)==0){
            //found right section in right file 
            s.code=code; 
            // cout<<"found section"<<endl; 
            // cout<<"section "+section<<endl; 
            // printCode(s.code); 
        }
    }
}


string Linker::findSymbolById(int id){
    for(auto s:localSymbolTable){
        if(s.second.symbolId==id)
            return s.second.symbolName; 
    }
    return ""; 
}

void Linker::printCode(string input){
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
    // std::cout << result << std::endl;
} 


string Linker::literalToHex(string token){
    int lit = stoi(token); 
    //literal must be withing range of 2 bytes 
    if(lit>WORD_MAX || lit<WORD_MIN){
        string msg ="Literal out of range! Error at line: "+ to_string(currLineNum);
        throw BadSynataxException(msg);
    }
    
    //TODO add hex regex 
    regex hexRegex("0x[0-9A-Fa-f]+"); 
    smatch sm; 
    if(!regex_search(token, sm, hexRegex)){
        //dec literal 
        token = decToHex(stoi(token)); 
    }
    // cout<<"Token is: "<<token<<endl; 
    regex hexPrefix("0x"); 
    token=regex_replace(token, hexPrefix, "");

    for (int i=0; i<token.length(); i++){
        token[i]=toupper(token[i]); 
    }
    for (int i=token.length(); i<4; i++){
        token.insert(0, "0"); 
    }
    return token;
}

string Linker::decToHex(int dec){
  std::stringstream ss;
  ss<< std::hex << dec; // int decimal_value
  std::string res ( ss.str() );
  return res; 
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
