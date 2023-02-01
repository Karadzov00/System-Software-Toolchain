
#include <regex>
#include <string>

using namespace std;

// cmd line args regex
regex optionArgRegex("^-o$");
regex outputFileRegex("^.*\\.o$");
regex inputFileRegex("^.*\\.s$");

// pretty formating regex
regex commentRegex("#.*");
regex emptyLineRegex("^\\s+$");
regex startLineSpaceRegex("^[ ]+");
regex endLineSpaceRegex("[ ]+$");
regex additionalSpaceRegex("[ ]{2,}");
regex spaceRegex("[ ]*");
regex commaSpaceRegex("[ ]?,[ ]?");

// util regex
string symbol = "[a-zA-Z][a-zA-Z0-9_]*";
regex symbolRegex("[a-zA-Z][a-zA-Z0-9_]*");
regex literalRegex("(0x[0-9A-Fa-f]+)|([0-9]+)"); 
regex labelRegex("^\\s*[a-zA-Z][a-zA-Z0-9_]*:");
regex onlyLabelInLineRegex("^\\s*[a-zA-Z][a-zA-Z0-9_]*:$");
regex somethingBeforeLabelRegex("\\S+[ ]+[a-zA-Z][a-zA-Z0-9_]*:");

// directive regex
regex globalRegex("\\.global ((([a-zA-Z][a-zA-Z0-9_]*), )*)(([a-zA-Z][a-zA-Z0-9_]*)+)");
regex externRegex("\\.extern ((([a-zA-Z][a-zA-Z0-9_]*), )*)(([a-zA-Z][a-zA-Z0-9_]*)+)");
regex sectionRegex("\\.section (([a-zA-Z][a-zA-Z0-9_]*)+)");
regex wordRegex(".word ([a-zA-Z][a-zA-Z0-9_]*|((\\b0x[a-fA-F0-9]+)|(\\b[-]?[0-9]+)))(, [a-zA-Z][a-zA-Z0-9_]*|(, ((\\b0x[a-fA-F0-9]+)|(\\b[-]?[0-9]+))))*");
regex skipRegex("\\.skip ((\\b0x[a-fA-F0-9]+)|(\\b[-]?[0-9]+))");
regex endRegex("\\.end$");

//register only regexs 
regex registersOnlyNoOperandRegex("^(halt|iret|ret)$");
regex registersOnlyOneOperandRegex("^(int|push|pop|not) ((r[0-7])|(pc|sp|psw))$"); 
regex registersOnlyTwoOperandRegex("^(xchg|add|sub|mul|div|cmp|and|or|xor|test|shl|shr) ((r[0-7])|(pc|sp|psw)), ((r[0-7])|(pc|sp|psw))$"); 
