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
    char got[256] {};
    int  *getFd(){ return fdInsert;}
    string insertS = "";

public:
    int openPipe();
    char *getFromPipe(){ return got;}
    int getFdRead(){ return this->getFd()[0];}
    int getFdWrite(){ return this->getFd()[1];}
    int getPid(){ return insertSon;}
    void setPid(pid_t newPid){ insertSon = newPid;}
    void waitInsert(unsigned &tabStatus);
    int writePipe();
    int readPipe();
    void closeWrite();
    void closeRead();
    string getString(){ return insertS;}
    void setString(string newString);
    query_result_t insertFunc(database_t *db, string query);
    

};

#endif
