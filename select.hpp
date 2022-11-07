#ifndef _SELECT_HPP
#define _SELECT_HPP
#include <csignal>
#include <iostream>
#include <iterator>
#include <unistd.h>

#include "query.hpp"
#include "db.hpp"

/*
 * select.hpp file for the program tinyDb
 * Groupe: Callens Hugo:000513303, Contuliano Bravo Rayan:000537050, Zaleski Arkadiusz:000516050 
 */




using namespace std;


class Select{
    int fdSelect[2];
    pid_t selectSon = 1;
    char got[256] {}; // Variable we use to store the content of the pipe
    string selectS = "";

    int  *getFd(){ return fdSelect;}
    char *getFromPipe(){ return got;}
    int getFdRead(){ return this->getFd()[0];}
    int getFdWrite(){ return this->getFd()[1];}
    void closeWrite();
    void closeRead();

public:
    int openPipe(); // Open a pipe with de fdSelect
    int getPid(){ return selectSon;}
    void setPid(pid_t newPid){ selectSon = newPid;}
    void waitSelect(unsigned &tabStatus); // Wait for the process Select to finish the command he was doing
    int writePipe();
    int readPipe();
    string getString(){ return selectS;}
    void setString(string newString);
    query_result_t selectFunc(database_t *db); // Function that calls the select Function in query.cpp
    

};

#endif
