#ifndef _SELECT_HPP
#define _SELECT_HPP
#include <csignal>
#include <iostream>
#include <iterator>
#include <unistd.h>

#include "query.hpp"
#include "db.hpp"




using namespace std;


class Select{
    int fdSelect[2];
    pid_t selectSon = 1;
    char got[256] {};
    int  *getFd(){ return fdSelect;}
    string selectS = "";

public:
    int openPipe();
    char *getFromPipe(){ return got;}
    int getFdRead(){ return this->getFd()[0];}
    int getFdWrite(){ return this->getFd()[1];}
    int getPid(){ return selectSon;}
    void setPid(pid_t newPid){ selectSon = newPid;}
    void waitSelect(unsigned &tabStatus);
    int writePipe();
    int readPipe();
    void closeWrite();
    void closeRead();
    string getString(){ return selectS;}
    void setString(string newString);
    query_result_t selectFunc(database_t *db, string query);
    

};

#endif
