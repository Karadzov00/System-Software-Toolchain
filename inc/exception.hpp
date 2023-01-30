#include <exception>
#include <string>

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

class EndDirecitveNotFound:public std::exception{
public:
  const char* what()const throw(){
    return "End directive not found!"; 
  }
};

class BadInputFileSyntax:public std::exception{
public:
  int line; 
  BadInputFileSyntax(int line):line(line){} 
  const char* what()const throw(){
    std::string s = std::to_string(line);
    std::string message = "Bad input file syntax! Error on line: "+s;
    return message.c_str(); 
  }
};

class BadSynataxException:public std::exception{
public:
  std::string message;
  int line; 
  BadSynataxException(std::string msg):message(msg){} 
  const char* what()const throw(){
    char* msg = new char[message.length()+1]; 
    strcpy(msg, message.c_str()); 
    return msg; 
  }
};

