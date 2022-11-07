#include <bits/types/struct_tm.h>
#include <csignal>
#include <cstddef>
#include <string>
#include <sys/mman.h>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>


#include "db.hpp"
#include "student.hpp"
#include "query.hpp"
#include "utils.hpp"
#include "update.hpp"
#include "delete.hpp"
#include "insert.hpp"
#include "select.hpp"

using namespace std;

int sigint = 1;

void signalHandler(int signum) {
   if (signum == SIGINT) { //^C
                           //Save and end programm
     cout <<  endl << "Handling SIGINT signal ... "<< endl;
     fclose(stdin); // Close stdin to get out of the waiting for input
     sigint = 0; 
   }else if (signum == SIGUSR1) {
     // Save db
     sigint = 2;
     cout << "Please press enter " << endl;
   }
}


int main(int argc, char const *argv[]) {

  //SIGUSR1 POUR MONITORING SYNC

  const char *dbPath = argv[argc-1];
  
  string tmp = std::string(dbPath);
  if (fileExist(tmp)) {
    cout << "File do not exists, please choose an existing binary file" << endl;
    return 1;
  }

  if (tmp.substr(tmp.size()-4, tmp.size()) != ".bin" ) { // Check if the file is a binary file
    cout << "You should pass a binary file for the database " << endl;
    return 1;

  }

  
  size_t allStudents = getNumberStudent(dbPath);
  if (!allStudents) {
    // If the binary file is empty
    allStudents = 1000;
    cout << "Your db is empty, the default size of the db is 1000 students" << endl;
  }
  void *ptrDb = mmap(nullptr, sizeof(database_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  void *ptrData = mmap(nullptr, sizeof(student_t)*allStudents*2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (ptrDb == MAP_FAILED) {
    perror("mmap Db");
    return 1;
  }
  if (ptrData == MAP_FAILED) {
    perror("mmap data");
    return 1;
  }

  database_t *db = (database_t*) ptrDb;
  db->data = (student_t*) ptrData;


  // int array to know if a child process has finished to do his command:: 0 working, 1 finished
  void *ptrTab = mmap(nullptr, sizeof(unsigned)*4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (ptrTab == MAP_FAILED) {
    perror("mmap tabStatus");
    return 1;
  }
  unsigned *tabStatus = (unsigned*) ptrTab;

  tabStatus[0] = 1; //SELECT 
  tabStatus[1] = 1; //UPDATE
  tabStatus[2] = 1; //DELETE
  tabStatus[3] = 1; //INSERT
  
  db_init(db, allStudents);
  db_load(db, dbPath);

  pid_t father = getpid();

  // CREATION DES FD:
  Select mySelect;
  Delete myDelete;
  Insert myInsert;
  Update myUpdate;
  

  if (mySelect.openPipe()) {return 1;}
  if (myDelete.openPipe()) {return 1;}
  if (myUpdate.openPipe()) {return 1;}
  if (myInsert.openPipe()) {return 1;}
  
  if (!createProcess(mySelect, myUpdate, myInsert, myDelete)){ return 1;}

  if (getpid() == father) {
  //Handling of signals for father
      signal(SIGINT, signalHandler);
      signal(SIGUSR1, signalHandler);
  }
  else {
  //Handling of signals for children
    signal(SIGINT, SIG_IGN); //Ignore sigint signal
  }

 while (cin and sigint) { //While stdin is on and we don't have a sigint signal
    if (getpid() == father) {
      //FATHER

      string transacS = "";

      if (sigint == 2) { // If we received the SIGUSR1 SIGNAL we wait for all process to finish
        mySelect.waitSelect(tabStatus[0]);
        myUpdate.waitUpdate(tabStatus[1]);
        myDelete.waitDelete(tabStatus[2]);
        myInsert.waitInsert(tabStatus[3]);

        cout << "Saving database to the disk ..." << endl;
        db_save(db, dbPath); // And we save the database
        cout << "Done ..." << endl;
        sigint = 1; // We update sigint to his default value
      }
      sleep(1); // Sleep father otherwise the input and output of father and children will mess each other
      getCommand(mySelect, myUpdate, myInsert, myDelete, transacS);

      if (mySelect.getString() != "") {
        mySelect.waitSelect(tabStatus[0]);
        tabStatus[0] = 0; // start working
        mySelect.writePipe();
        mySelect.setString("");

      }if (myUpdate.getString() != "") {
        myUpdate.waitUpdate(tabStatus[1]);
        tabStatus[1] = 0;
        myUpdate.writePipe();
        myUpdate.setString("");

      }if (myInsert.getString() != "") {
        myInsert.waitInsert(tabStatus[3]);
        tabStatus[3] = 0;
        myInsert.writePipe();
        myInsert.setString("");
      }if (myDelete.getString() != ""){
        myDelete.waitDelete(tabStatus[2]);
        tabStatus[2] = 0;
        myDelete.writePipe();
        myDelete.setString("");

      }if (transacS != "") {
        mySelect.waitSelect(tabStatus[0]);
        myUpdate.waitUpdate(tabStatus[1]);
        myDelete.waitDelete(tabStatus[2]);
        myInsert.waitInsert(tabStatus[3]);
      }

    }
    else {
      //SON READ
      if (mySelect.getPid() == 0) {
        mySelect.readPipe();
        query_result_t ret = mySelect.selectFunc(db);
        log_query(&ret);
        if (ret.status != QUERY_FAILURE and ret.status != UNRECOGNISED_FIELD) {
          delete [] ret.students;
        }
        tabStatus[0] = 1; // finish working
      }
      else if (myUpdate.getPid() == 0) {
        myUpdate.readPipe();
        query_result_t ret = myUpdate.updateFunc(db);
        log_query(&ret);
        if (ret.status != QUERY_FAILURE and ret.status != UNRECOGNISED_FIELD) {
          delete [] ret.students;
        }
        tabStatus[1] = 1;

      }
      else if (myDelete.getPid()== 0){
        myDelete.readPipe();
        query_result_t ret = myDelete.deleteFunc(db);
        log_query(&ret);
        if (ret.status != QUERY_FAILURE and ret.status != UNRECOGNISED_FIELD) {
          delete [] ret.students;
        }
        tabStatus[2] = 1;
      }
      else if (myInsert.getPid() == 0) {
        myInsert.readPipe();
        query_result_t ret = myInsert.insertFunc(db);
        log_query(&ret);
        if (ret.status != QUERY_FAILURE and ret.status != UNRECOGNISED_FIELD) {
          delete [] ret.students;
        }
        tabStatus[3] = 1;
      }  
    }
  }
  if (!sigint) {
    // if we received the sigint signal we wait for children to end what they are doing
        mySelect.waitSelect(tabStatus[0]);
        myUpdate.waitUpdate(tabStatus[1]);
        myDelete.waitDelete(tabStatus[2]);
        myInsert.waitInsert(tabStatus[3]);
  }

  cout << "Saving the database to the disk..." << endl;
  db_save(db, dbPath);
  cout << "Done !" << endl;
  munmap(db->data, sizeof(student_t) * db->psize);
  munmap(db, sizeof(database_t));
  cout << "Bye bye" << endl;
  return 0;
}
