#include "utils.hpp"

#include <cstddef>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>

#include "query.hpp"
#include "student.hpp"
#include "select.hpp"
#include "update.hpp"
#include "delete.hpp"
#include "insert.hpp"

using namespace std;

size_t safe_read(int fd, void* buffer, size_t nbytes) {
  ssize_t nbytes_read = read(fd, buffer, nbytes);
  if (nbytes_read < 0) {
    perror("read error: ");
    exit(1);
  }
  
  return (size_t)nbytes_read;
}

size_t safe_write(int fd, const void* buffer, size_t nbytes) {
  ssize_t bytes_written = write(fd, buffer, nbytes);
  if (bytes_written < 0) {
    perror("write: ");
    exit(1);
  }
  return (size_t)bytes_written;
}

void log_query(query_result_t* result) {

  // TO CHECK IF THE DIR LOGS EXISTS
  struct stat buffer;
  string logs = "logs";
  int pid = 1;
  
  if (stat(logs.c_str(), &buffer) != 0) {
    //cout << "The dir doesn't exist" << endl;
    pid = fork();
    if (pid == -1) {
      cout << "Error with the fork on query" << endl;
    }
  }
  //else {
      //cout << "The dir exists" << endl;
      
  //}  

  if (pid == 0) {
    // Child process
    execlp("mkdir", "mkdir", "logs", NULL);
  }else{
    // Parent process
    wait(NULL);
    char buffer[512];
    if (result->status == QUERY_SUCCESS) {
      char filename[512];
      char type[256];
      strcpy(type, result->query);
      type[6] = '\0';
      sprintf(filename, "logs/%ld-%s.txt", result->start_ns, type);
      //printf("%s\n", filename);
      cout << filename << endl;
      FILE* f = fopen(filename, "w");
      float duration = (float)(result->end_ns - result->start_ns) / 1.0e6;
      sprintf(buffer, "Query \"%s\" completed in %fms with %ld results.\n", result->query, duration, result->lsize);
      fwrite(buffer, sizeof(char), strlen(buffer), f);
      if (result->lsize > 0) {
        for (size_t i = 0; i < result->lsize; i++) {
          student_to_str(buffer, &result->students[i]);
          fwrite(buffer, sizeof(char), strlen(buffer), f);
          fwrite("\n", sizeof(char), 1, f);
        }
      }
      fclose(f);
    }else if (result->status == UNRECOGNISED_FIELD) {
    fprintf(stderr, "Unrecognized field in query %s\n", result->query);
    }
  }

}

size_t getNumberStudent(const char *path){
    std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    int size = in.tellg()/sizeof(student_t);
    //db_init(db, size);


    std::cout << "len file : " << in.tellg() << std::endl;

    std::cout << "len student : " << sizeof(student_t) << std::endl;

    std::cout << "Preshot nb étudiants : len file / len sutdent = " << in.tellg() / sizeof(student_t) << std::endl;

    return size;

}

bool getType(std::string &command, Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete){
  if (command.substr(0, 6) == "select") {
    //select = command;
    mySelect.setString(command);
  }else if (command.substr(0, 6) == "update") {
    myUpdate.setString(command);
  }else if (command.substr(0, 6) == "insert") {
    myInsert.setString(command);
  }else if (command.substr(0, 6) == "delete"){
    myDelete.setString(command);
  }else {
    return false;
  }
  return true;
}


bool getCommand(Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete, std::string &transaction){
  string commandLine;
  usleep(250000);
  cout << "> ";
  getline(cin, commandLine);
  if (commandLine.substr(0, 11) == "transaction") {
    transaction = "transaction";
    return true;
    
  }else if (getType(commandLine, mySelect, myUpdate, myInsert, myDelete)) {
    return true;
  }
  else if (commandLine == "") {
    return false;
  }else {
    cout << "Please enter one of these instruction: "
    "select, update, insert, delete or transaction if you want to end the command " << endl;
    return false;
  }
}


 
bool createProcess(Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete){
  mySelect.setPid(fork());
  if (mySelect.getPid() < 0) { perror("Fork() Select"); return false;}
  else if (!mySelect.getPid()) {/*IN SELECT SON PROCESS*/ /*cout << "Select : " << getpid() << " père : " << getppid()<< endl;*/}
  else {/*IN PARENT PROCESS*/
    myUpdate.setPid(fork());
    if (myUpdate.getPid() < 0) {perror("fork() Update"); return false;}
    else if (!myUpdate.getPid()) {/*IN UPDATE SON PROCESS*/  /*cout << "Update : " << getpid() << " père : " << getppid()<< endl;*/}    
    else { //[>IN PARENT PROCESS<]
      myDelete.setPid(fork());
      if (myDelete.getPid() < 0) {perror("fork() Delete"); return false;}
      else if (!myDelete.getPid()) {/*[>IN DELETE SON PROCESS<]  cout << "Delete : " << getpid() << " père : " << getppid()<< endl;*/}    
      else {//[>IN PARENT PROCESS<]
        myInsert.setPid(fork());
        if (myInsert.getPid() < 0) {perror("fork() Insert"); return false;}
        else if (!myInsert.getPid()) {/*[>IN INSERT SON PROCESS<]  cout << "Insert : " << getpid() << " père : " << getppid()<< endl;*/}
      }
    }
  }
  return true;

}
