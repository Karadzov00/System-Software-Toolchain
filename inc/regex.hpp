
#include <regex>
#include <string>

using namespace std; 

//cmd line args regex 
regex optionArgRegex("^-o$"); 
regex outputFileRegex("^.*\\.o$"); 
regex inputFileRegex("^.*\\.s$"); 

//pretty formating regex 
regex commentRegex("#.*");
regex emptyLineRegex("^\\s+$");  
regex startLineSpaceRegex("^[ ]+");
regex endLineSpaceRegex("[ ]+$");
regex additionalSpaceRegex("[ ]{2,}");
regex spaceRegex("[ ]*"); 
regex commaSpaceRegex("[ ]?,[ ]?"); 

string symbol = "[a-zA-Z][a-zA-Z0-9_]*";
regex label("^\\s*[a-zA-Z][a-zA-Z0-9_]*:");
regex onlyLabelInLineRegex("^\\s*[a-zA-Z][a-zA-Z0-9_]*:$"); 
regex somethingBeforeLabelRegex("\\S+[ ]+[a-zA-Z][a-zA-Z0-9_]*:"); 

//directive regex 
regex globalRegex("\\.global ((([a-zA-Z][a-zA-Z0-9_]*), )*)(([a-zA-Z][a-zA-Z0-9_]*)+)");
regex externRegex("\\.extern ((([a-zA-Z][a-zA-Z0-9_]*), )*)(([a-zA-Z][a-zA-Z0-9_]*)+)");
regex sectionRegex("\\.section (([a-zA-Z][a-zA-Z0-9_]*)+)");
regex wordRegex("");
regex skipRegex(".skip ((\\b0x[a-fA-F0-9]+)|(\\b[-]?[0-9]+))");
regex endRegex(".end$"); 