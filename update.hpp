#ifndef _UPDATE_HPP
#define _UPDATE_HPP
#include <csignal>
#include <iostream>
#include <iterator>
#include <unistd.h>

#include "query.hpp"
#include "db.hpp"


/*
 * update.hpp file for the program tinyDb
 * Groupe: Callens Hugo:000513303, Contuliano Bravo Rayan:000537050, Zaleski Arkadiusz:000516050 
 */



using namespace std;


class Update{
    int fdUpdate[2];
    pid_t updateSon = 1;
    char got[256] {};  // Variable we use to store the content of the pipe
    string updateS = ""; // String that will contain the command for Update
                         //
    int  *getFd(){ return fdUpdate;}
    char *getFromPipe(){ return got;}
    int getFdRead(){ return this->getFd()[0];}
    int getFdWrite(){ return this->getFd()[1];}
    void closeWrite();
    void closeRead();

public:
    int openPipe(); // Open a pipe with fdUpdate
    int getPid(){ return updateSon;}
    void setPid(pid_t newPid){ updateSon = newPid;}
    void waitUpdate(unsigned &tabStatus); // Wait for the process Update to end the command he was doing
    int writePipe();
    int readPipe();
    string getString(){ return updateS;}
    void setString(string newString);
    query_result_t updateFunc(database_t *db); // Function that calls the update function in query.cpp
};

#endif
