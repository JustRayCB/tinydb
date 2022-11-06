#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>


#include "query.hpp"
#include "update.hpp"


using namespace std;


int Update::openPipe(){
    if (pipe(this->getFd()) < 0){ 
        perror("Pipe() Update");
        return 1;
    }
    return 0;
}

void Update::waitUpdate(unsigned &tabStatus){
  while (tabStatus == 0) { // If the process of Update has endend the command he
                           // was doing he update
                           // the tabStatus to 1
        cout << "Waiting for process Update to  finish" << endl;
        usleep(500000);
    }
}

int Update::writePipe(){
    char toChar[256]; // Variable use to write in the pipe cause we cant write strings
    this->closeRead();
    strcpy(toChar, this->getString().c_str());
    if (write(this->getFdWrite(), &toChar, 256) == -1){
        perror("Write Update FD");
        return 1;
    }
    return 0;
}

int Update::readPipe(){
    memset(this->getFromPipe(), 0, 256); // Empty the Char array
    this->closeWrite();
    if (read(this->getFdRead(), this->getFromPipe(), 256) == -1){
        return 1;
    }
    return 0;
}

void Update::closeWrite(){
    close(this->getFdWrite());
}

void Update::closeRead(){
    close(this->getFdRead());
}

query_result_t Update::updateFunc(database_t *db){
    string command = this->getFromPipe();
    cout << "Running query '" << command << "'..." <<  endl;
    query_result_t ret = update(db, command.substr(7, command.length()));
    return ret;
}

void Update::setString(string newString){
    updateS = newString;
}


