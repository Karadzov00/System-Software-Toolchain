#include <exception>

class BadCmdArgsException:public std::exception{
public:
  const char* what()const throw(){
    return "Wrong input arguments in command line!"; 
  }
};

class FileNotOpenException:public std::exception{
public:
  const char* what()const throw(){
    return "File is not opened properly!"; 
  }
};