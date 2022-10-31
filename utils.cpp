#include "utils.hpp"

#include <cstddef>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>

#include "query.hpp"
#include "student.hpp"

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
