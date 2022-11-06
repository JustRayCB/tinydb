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
  while (tabStatus == 0) { // If the process of Delete has endend the command he
                           // was doing he update
                           // the tabStatus to 1
        cout << "Waiting for process Delete to  finish" << endl;
        usleep(500000);
    }
}

int Delete::writePipe(){
    char toChar[256]; // Variable use to write in the pipe cause you cant write a string
    this->closeRead();
    strcpy(toChar, this->getString().c_str());
    if (write(this->getFdWrite(), &toChar, 256) == -1){
        perror("Write Delete FD");
        return 1;
    }
    return 0;
}

int Delete::readPipe(){
    memset(this->getFromPipe(), 0, 256); // Empty the char array
    this->closeWrite();
    if (read(this->getFdRead(), this->getFromPipe(), 256) == -1){
        perror("Read Delete fd");
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

query_result_t Delete::deleteFunc(database_t *db){
    string command = this->getFromPipe(); // Convert got to string cause it's easier with strings
    cout << "Running query '" << command << "'..." <<  endl;
    query_result_t ret = deletion(db, command.substr(7, command.length()));
    return ret;
}

void Delete::setString(string newString){
        deleteS = newString;
}


