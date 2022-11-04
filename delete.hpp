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
    char got[256] {};
    int  *getFd(){ return fdDelete;}
    string deleteS = "";

public:
    int openPipe();
    char *getFromPipe(){ return got;}
    int getFdRead(){ return this->getFd()[0];}
    int getFdWrite(){ return this->getFd()[1];}
    int getPid(){ return deleteSon;}
    void setPid(pid_t newPid){ deleteSon= newPid;}
    void waitDelete(unsigned &tabStatus);
    int writePipe();
    int readPipe();
    void closeWrite();
    void closeRead();
    string getString(){ return deleteS;}
    void setString(string newString);
   query_result_t deleteFunc(database_t *db, string query);
    

};

#endif
