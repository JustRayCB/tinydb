#ifndef _INSERT_HPP
#define _INSERT_HPP
#include <csignal>
#include <iostream>
#include <iterator>
#include <unistd.h>

#include "query.hpp"
#include "db.hpp"




using namespace std;


class Insert{
    int fdInsert[2];
    pid_t insertSon = 1;
    char got[256] {}; // Variable we use to store the content of the file
    string insertS = ""; // String that will contain the command for Insert

    int  *getFd(){ return fdInsert;} // Return the File descriptor of the class
    char *getFromPipe(){ return got;} 
    int getFdRead(){ return this->getFd()[0];}
    int getFdWrite(){ return this->getFd()[1];}
    void closeWrite();
    void closeRead();

public:
    int openPipe(); // Open the pipe with fdInsert
    int getPid(){ return insertSon;}
    void setPid(pid_t newPid){ insertSon = newPid;}
    void waitInsert(unsigned &tabStatus); // Wait for the process to finish what he is doing
    int writePipe();
    int readPipe();
    string getString(){ return insertS;}
    void setString(string newString);
    query_result_t insertFunc(database_t *db); // Function that call the Insert function in query.cpp
    

};

#endif
