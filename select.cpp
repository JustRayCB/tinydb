#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>


#include "query.hpp"
#include "select.hpp"


using namespace std;


int Select::openPipe(){
    if (pipe(this->getFd()) < 0){ 
        perror("Pipe() Select");
        return 1;
    }
    return 0;
}

void Select::waitSelect(unsigned &tabStatus){
  while (tabStatus == 0) { // If the process Select has endend the command he
                           // was doing he update
                           // the tabStatus to 1
        cout << "Waiting for process Select to  finish" << endl;
        usleep(500000);
    }
}

int Select::writePipe(){
    char toChar[256]; // Variable we use to write in the Pipe cause we cant write strings
    this->closeRead();
    strcpy(toChar, this->getString().c_str());
    if (write(this->getFdWrite(), &toChar, 256) == -1){
        perror("Write Selec FD");
        return 1;
    }
    return 0;
}

int Select::readPipe(){
    memset(this->getFromPipe(), 0, 256); // Empty char array
    this->closeWrite();
    if (read(this->getFdRead(), this->getFromPipe(), 256) == -1){
        return 1;
    }
    return 0;
}

void Select::closeWrite(){
    close(this->getFdWrite());
}

void Select::closeRead(){
    close(this->getFdRead());
}

query_result_t Select::selectFunc(database_t *db){
    string command = this->getFromPipe(); // Convert got to string cause it's easier with strings
    query_result_t ret = select(db, command.substr(7, command.length()));
    return ret;
}

void Select::setString(string newString){
        selectS = newString;
}


