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
     // DOIT ARRETER LES PROCESS ET SAVE LA DB;
     cout <<  endl << "Handling SIGINT signal ... "<< endl;
     //fclose(stdin);
     sigint = 0;
   }else if (signum == SIGUSR1) {
     //doit save la db
     sigint = 2;
     cout << "Please press enter " << endl;

   }
}


int main(int argc, char const *argv[]) {

  //SIGUSR1 POUR MONITORING SYNC

  const char *dbPath = argv[argc-1];
  string tmp = std::string(dbPath);
  if (tmp.substr(tmp.size()-4, tmp.size()) != ".bin" ) {
    cout << "You should pass a binary file for the database " << endl;
    return 1;

  }

  //MEMOIRE PARTAGEE
  size_t allStudents = getNumberStudent(dbPath);
  // PREMIER PARAMETRE DE MMAP nullptr -> Laisse choisir à l'os l'address de la
  // mémoire partagée
  // Si on y met l'address de la db on "essaye" de forcer l'os à mettre la
  // mémoire partagée là ou il y la db
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


  // tableau pour savoir si un processus à terminé de faire son travail:: 0 en train de travailler
  // 1 finis de travailler
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

  //Handling of signals
  if (getpid() == father) {
      signal(SIGINT, signalHandler);
      signal(SIGUSR1, signalHandler);
  }
  else {
    signal(SIGINT, SIG_IGN);
  }

 while (cin and sigint) {
    if (getpid() == father) {
      //FATHER
      string /*selectS = "", updateS = "", insertS = "", delS = "",*/ transacS = "";

      usleep(500000); // sleep during 0.5 sec
      if (sigint == 2) {
        mySelect.waitSelect(tabStatus[0]);
        myUpdate.waitUpdate(tabStatus[1]);
        myDelete.waitDelete(tabStatus[2]);
        myInsert.waitInsert(tabStatus[3]);

        cout << "Saving database to the disk ..." << endl;
        db_save(db, dbPath);
        cout << "Done ..." << endl;
        sigint = 1;
      }
      getCommand(mySelect, myUpdate, myInsert, myDelete, transacS);

      if (mySelect.getString() != "") {
        mySelect.waitSelect(tabStatus[0]);
        tabStatus[0] = 0;
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
        cout << "Select :  " << getpid() << " father : " << getppid() << endl;
        mySelect.readPipe();
        string command = mySelect.getFromPipe();
        query_result_t ret;
        ret = mySelect.selectFunc(db, command.substr(7, command.length()));
        log_query(&ret);
        if (ret.status != QUERY_FAILURE) {
          delete [] ret.students;
        }
        sleep(10);
        tabStatus[0] = 1;
      }
      else if (myUpdate.getPid() == 0) {
        cout << "UPDATE :  " << getpid() << " father : " << getppid() << endl;
        myUpdate.readPipe();
        string command = myUpdate.getFromPipe();
        query_result_t ret;
        ret = myUpdate.updateFunc(db, command.substr(7, command.length()) );
        log_query(&ret);
        //sleep(15);
        if (ret.status != QUERY_FAILURE) {
          delete [] ret.students;
        }

        sleep(10);
        tabStatus[1] = 1;

      }
      else if (myDelete.getPid()== 0){
        cout << "DELETE :  " << getpid() << " father : " << getppid() << endl;
        myDelete.readPipe();
        string command = myDelete.getFromPipe();
        query_result_t ret;
        ret = myDelete.deleteFunc(db, command.substr(7, command.length()) );
        log_query(&ret);
        if (ret.status != QUERY_FAILURE) {
          delete [] ret.students;
        }
        tabStatus[2] = 1;
      }
      else if (myInsert.getPid() == 0) {
        cout << "INSERT :  " << getpid() << " father : " << getppid() << endl;
        myInsert.readPipe();
        string command = myInsert.getFromPipe();
        query_result_t ret;
        ret = myInsert.insertFunc(db, command.substr(7, command.length()) );
        log_query(&ret);
        if (ret.status != QUERY_FAILURE) {
          delete [] ret.students;
        }
        tabStatus[3] = 1;
      }  
    }
  }
  if (!sigint) {
    cout << "Here whit pid : " << getpid() << endl;
        mySelect.waitSelect(tabStatus[0]);
        myUpdate.waitUpdate(tabStatus[1]);
        myDelete.waitDelete(tabStatus[2]);
        myInsert.waitInsert(tabStatus[3]);
  }

  cout << "Saving the database to the disk" << endl;
  db_save(db, dbPath);
  munmap(db->data, sizeof(student_t) * db->psize);
  munmap(db, sizeof(database_t));
  printf("Bye bye!\n");
  return 0;
}
