#include <exception>

class BadCmdArgsException:public std::exception{
public:
  const char* what()const throw(){
    return "Wrong input arguments in command line!"; 
  }
};