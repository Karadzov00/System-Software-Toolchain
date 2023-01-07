
#include <regex>
#include <string>

using namespace std; 

//cmd line args regex 
regex optionArgRegex("^-o$"); 
regex outputFileRegex("^.*\\.o$"); 
regex inputFileRegex("^.*\\.s$"); 

regex commentRegex("#.*");
regex emptyLineRegex("^//s$");  
regex startLineSpaceRegex("^[ ]+");
regex endLineSpaceRegex("[ ]+$");
regex additionalSpaceRegex("[ ]{2,}");
regex spaceRegex("[ ]*"); 
regex commaSpaceRegex("[ ]?,[ ]?"); 

string symbol = "[a-zA-Z][a-zA-Z0-9]*";
regex label("^\\s*[a-zA-Z][a-zA-Z0-9]*:");
regex onlyLabelInLineRegex("^\\s*[a-zA-Z][a-zA-Z0-9]*:$"); 

