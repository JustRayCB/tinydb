#ifndef _DELETE_HPP
#define _DELETE_HPP
#include <csignal>
#include <iostream>
#include <iterator>
#include <unistd.h>

#include "query.hpp"
#include "db.hpp"




using namespace std;


class Delete{
    int fdDelete[2];
    pid_t deleteSon = 1;
    char got[256] {}; // Variable we use to store the content of the pipe
    string deleteS = ""; // String that whill contain the command for Delete

    int  *getFd(){ return fdDelete;} // Function that return the file descriptor of the Class
    char *getFromPipe(){ return got;} // Return the Char array we use to store the content of the pipe
    int getFdRead(){ return this->getFd()[0];} 
    int getFdWrite(){ return this->getFd()[1];}
    void closeWrite();
    void closeRead();

public:
    int openPipe(); // Open a pipe with the fdDelete
    int getPid(){ return deleteSon;}
    void setPid(pid_t newPid){ deleteSon= newPid;}
    void waitDelete(unsigned &tabStatus); // Wait for the process to finish what he is doing
    int writePipe();
    int readPipe();
    string getString(){ return deleteS;}
    void setString(string newString);
    query_result_t deleteFunc(database_t *db); // Function that calls the deletion function in query.cpp
    

};

#endif
