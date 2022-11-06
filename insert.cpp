#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>


#include "query.hpp"
#include "insert.hpp"


using namespace std;


int Insert::openPipe(){
    if (pipe(this->getFd()) < 0){ 
        perror("Pipe() Insert");
        return 1;
    }
    return 0;
}

void Insert::waitInsert(unsigned &tabStatus){
  while (tabStatus == 0) { // If the process of Insert has endend the command he
                           // was doing he update
                           // the tabStatus to 1
        cout << "Waiting for process Insert to  finish" << endl;
        usleep(500000);
    }
}

int Insert::writePipe(){
    char toChar[256]; //Variable we use to write in the pipe cause we cant write strings
    this->closeRead();
    strcpy(toChar, this->getString().c_str());
    if (write(this->getFdWrite(), &toChar, 256) == -1){
        perror("Write Insert FD");
        return 1;
    }
    return 0;
}

int Insert::readPipe(){
    memset(this->getFromPipe(), 0, 256); // Empty the Char array
    this->closeWrite();
    if (read(this->getFdRead(), this->getFromPipe(), 256) == -1){
        perror("Read Insert fd");
        return 1;
    }
    return 0;
}

void Insert::closeWrite(){
    close(this->getFdWrite());
}

void Insert::closeRead(){
    close(this->getFdRead());
}

query_result_t Insert::insertFunc(database_t *db){
    string command = this->getFromPipe();
    cout << "Running query '" << command << "'..." <<  endl;
    query_result_t ret = insert(db, command.substr(7, command.length()));
    return ret;
}

void Insert::setString(string newString){
        insertS = newString;
}


