#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>


#include "query.hpp"
#include "delete.hpp"


using namespace std;


int Delete::openPipe(){
    if (pipe(this->getFd()) < 0){ 
        perror("Pipe() Delete");
        return 1;
    }
    return 0;
}

void Delete::waitDelete(unsigned &tabStatus){
    while (tabStatus == 0) {
        cout << "Waiting for process Delete to  finish" << endl;
        usleep(500000);
    }
}

int Delete::writePipe(){
    char toChar[256];
    this->closeRead();
    strcpy(toChar, this->getString().c_str());
    if (write(this->getFdWrite(), &toChar, 256) == -1){
        perror("Write Delete FD");
        return 1;
    }
    return 0;
}

int Delete::readPipe(){
    memset(this->getFromPipe(), 0, 256);
    this->closeWrite();
    if (read(this->getFdRead(), this->getFromPipe(), 256) == -1){
        return 1;
    }
    return 0;
}

void Delete::closeWrite(){
    close(this->getFdWrite());
}

void Delete::closeRead(){
    close(this->getFdRead());
}

query_result_t Delete::deleteFunc(database_t *db, string query){
    string command = this->getFromPipe();
    query_result_t ret = deletion(db, command.substr(7, command.length()));
    return ret;
}

void Delete::setString(string newString){
        deleteS = newString;
}


