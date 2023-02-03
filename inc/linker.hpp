#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <fstream>
#include <map>


#include "exception.hpp"

using namespace std; 

class Linker{
private:

    vector<string>inputFiles; 
    string outputFile; 

public:
  static bool checkCmdArguments(int argc, char* argv[]); 
  void openParseFile(); 

};