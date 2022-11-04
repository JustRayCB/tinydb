#ifndef _UPDATE_HPP
#define _UPDATE_HPP
#include <csignal>
#include <iostream>
#include <iterator>
#include <unistd.h>

#include "query.hpp"
#include "db.hpp"




using namespace std;


class Update{
    int fdUpdate[2];
    pid_t updateSon = 1;
    char got[256] {};
    int  *getFd(){ return fdUpdate;}
    string updateS = "";

public:
    int openPipe();
    char *getFromPipe(){ return got;}
    int getFdRead(){ return this->getFd()[0];}
    int getFdWrite(){ return this->getFd()[1];}
    int getPid(){ return updateSon;}
    void setPid(pid_t newPid){ updateSon = newPid;}
    void waitUpdate(unsigned &tabStatus);
    int writePipe();
    int readPipe();
    void closeWrite();
    void closeRead();
    string getString(){ return updateS;}
    void setString(string newString);
    query_result_t updateFunc(database_t *db, string query);
    

};

#endif
