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
#include <cstdlib>


#include "db.hpp"
#include "student.hpp"
#include "query.hpp"
#include "utils.hpp"

using namespace std;

int sigint = 1;

void isWorking(database_t *db){
  char buffer[512];
  int count = 0;
  for (size_t idx=0; idx < 10; idx++) {
    student_t one = db->data[idx];
    if (strcmp(one.section, "info") == 0) {
            count++;
    }
    student_to_str(buffer, &one);
    std::cout << buffer << std::endl;

    memset(buffer, 0, sizeof(buffer));
    
  }

  cout << "Le nombre de personnes en info: " << count <<  endl;

}

void signalHandler(int signum) {
   if (signum == SIGINT) {
     // DOIT ARRETER LES PROCESS ET SAVE LA DB;
     cout <<  endl << "Handling SIGINT signal ... "<< endl;
     fclose(stdin);
     sigint = 0;
   }else if (signum == SIGUSR1) {
     //doit save la db
     sigint = 2;
     cout << "Please press enter " << endl;

   }
   //exit(signum);
}


int main(int argc, char const *argv[]) {

  //SIGUSR1 POUR MONITORING SYNC

  const char *db_path = argv[argc-1];
  string tmp = std::string(db_path);
  cout << tmp << endl;
  if (tmp.substr(tmp.size()-4, tmp.size()) != ".bin" ) {
    cout << "You should pass a binary file for the database " << endl;
    return 1;

  }

  //MEMOIRE PARTAGEE
  size_t allStudents = getNumberStudent(db_path);
  // PREMIER PARAMETRE DE MMAP nullptr -> Laisse choisir à l'os l'address de la
  // mémoire partagée
  // Si on y met l'address de la db on "essaye" de forcer l'os à mettre la
  // mémoire partagée là ou il y la db
  database_t *db= (database_t*)mmap(nullptr, sizeof(database_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  db->data = (student_t*)mmap(nullptr, sizeof(student_t)*allStudents*2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
  //cout << sizeof(*db) << endl;

  // tableau pour savoir si un processus à terminé de faire son travail:: 0 en train de travailler
  // 1 finis de travailler
  unsigned *tabStatus = (unsigned*)mmap(nullptr, sizeof(unsigned)*4, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
  tabStatus[0] = 1; //SELECT 
  tabStatus[1] = 1; //UPDATE
  tabStatus[2] = 1; //DELETE
  tabStatus[3] = 1; //INSERT
  
  db_init(db, allStudents);
  db_load(db, db_path);

  //isWorking(db);
  pid_t father = getpid();



  //database_t *share = (database_t*)mmap(nullptr, sizeof(database_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  //share = db;
  //if (share == MAP_FAILED) {
    //cout << "ERROR WITH MMAP" << endl;
    //return 4;
  //}

  // CREATION DES FD:
  int fdSelect[2], fdUpdate[2], fdDelete[2], fdInsert[2];
  // CREATION PID
  pid_t selectSon = 1, updateSon = 1, deleteSon = 1, insertSon = 1;

  if (pipe(fdSelect) < 0) {perror("Pipes() Select");}
  if (pipe(fdUpdate) < 0) {perror("Pipes() Update");}
  if (pipe(fdDelete) < 0) {perror("Pipes() Delete");}
  if (pipe(fdInsert) < 0) {perror("Pipes() Insert");}
  
  if (!createProcess(selectSon, updateSon, insertSon, deleteSon)){ return 1;}
  //// faut utiliser fork
  if (getpid() == father) {
      signal(SIGINT, signalHandler);
      signal(SIGUSR1, signalHandler);
  }
 while (cin and sigint) {
    if (getpid() == father) {
      //FATHER WRITE
      string selectS = "", updateS = "", insertS = "", delS = "", transacS = "";
      //sleep(1);
      usleep(500000); // sleep during 0.25 sec
      //isWorking(db);
      if (sigint == 2) {
        while (tabStatus[0] == 0 or tabStatus[1] == 0 or tabStatus[2] == 0 or tabStatus[3] == 0) {
          cout << "Waiting for process to  finish" << endl;
          usleep(250000); // sleep during 0.25 sec
        }
        cout << "Saving database to the disk ..." << endl;
        db_save(db, db_path);
        cout << "Done ..." << endl;
        sigint = 1;
      }
      getcommand(selectS, updateS, insertS, delS, transacS);

      if (selectS != "") {
        while (tabStatus[0] == 0) {
          cout << "Waiting for process Select to  finish" << endl;
          usleep(250000); // sleep during 0.25 sec

        }
        tabStatus[0] = 0;
        char real[256];
        strcpy(real, selectS.c_str());
        write(fdSelect[1], &real, 256);
        //close(fdSelect[1]);
      }if (updateS != "") {
        while (tabStatus[2] == 0) {
          cout << "Waiting for process Update to  finish" << endl;
          usleep(250000); // sleep during 0.25 sec

        }

        tabStatus[1] = 0;
        char real[256];
        strcpy(real, updateS.c_str());
        write(fdUpdate[1], &real, 256);

      }if (insertS != "") {
        while (tabStatus[3] == 0) {
          cout << "Waiting for process Insert to  finish" << endl;
          usleep(250000); // sleep during 0.25 sec

        }

        tabStatus[3] = 0;
        char real[256];
        strcpy(real, insertS.c_str());
        write(fdInsert[1], &real, 256);
      }if (delS != ""){
        while (tabStatus[2] == 0) {
          cout << "Waiting for process Delete  to  finish" << endl;
          usleep(250000); // sleep during 0.25 sec

        }

        tabStatus[2] = 0;
        char real[256];
        strcpy(real, delS.c_str());
        write(fdDelete[1], &real, 256);
      }if (transacS != "") {
        cout << "tab[";
        for (int idx = 0; idx <4; idx++) {
          cout << tabStatus[idx] <<", ";

        }
        cout << "]" << endl;

        while (tabStatus[0] == 0 or tabStatus[1] == 0 or tabStatus[2] == 0 or tabStatus[3] == 0) {
          cout << "Waiting for process to  finish" << endl;
          usleep(250000); // sleep during 0.25 sec
        }
      }

    }
    else {
      //SON READ
      if (selectSon == 0) {
        //cout << "Select :  " << getpid() << " father : " << getppid() << endl;
        //cout << "SELECT" << endl;
        char got[256];
        close(fdSelect[1]);
        read(fdSelect[0], &got, 256);
        //cout <<"I have received " <<  got << endl;
        //close(fdSelect[1]);
        string command = got;
        //cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
        query_result_t ret;
        ret = select(db, command.substr(7, command.length()));
        //sleep(10);
        log_query(&ret);
        delete [] ret.students;
        tabStatus[0] = 1;
      }
      else if (updateSon == 0) {
        //cout << "UPDATE :  " << getpid() << " father : " << getppid() << endl;
        //cout << "UPDATE" << endl;
        char got[256];
        close(fdUpdate[1]);
        read(fdUpdate[0], &got, 256);
        //cout << "I have received " << got << endl;
        string command = got;
        //cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
        query_result_t ret;
        ret = update(db, command.substr(7, command.length()) );
        log_query(&ret);
        //sleep(15);
        delete [] ret.students;
        char buffer[256];student_to_str(buffer, &db->data[0]);
        tabStatus[1] = 1;

      }
      else if (deleteSon == 0){
        //cout << "DELETE :  " << getpid() << " father : " << getppid() << endl;
        char got[256];
        //cout << "DELETE" << endl;
        close(fdDelete[1]);
        read(fdDelete[0], &got, 256);
        string command = got;
        query_result_t ret;
        ret = deletion(db, command.substr(7, command.length()) );
        log_query(&ret);
        delete [] ret.students;
        tabStatus[2] = 1;
      }
      else if (insertSon == 0) {
        //cout << "INSERT :  " << getpid() << " father : " << getppid() << endl;
        char got[256];
        close(fdInsert[1]);
        read(fdInsert[0], &got, 256);
        string command = got;
        query_result_t ret;
        ret = insert(db, command.substr(7, command.length()) );
        log_query(&ret);
        delete [] ret.students;
        tabStatus[3] = 1;
      }  
    }
  }
  if (!sigint) {
        while (tabStatus[0] == 0 or tabStatus[1] == 0 or tabStatus[2] == 0 or tabStatus[3] == 0) {
          cout << "Waiting for process to  finish" << endl;
          usleep(250000); // sleep during 0.25 sec
        }
  }

    // Il y a sans doute des choses à faire ici...
  cout << "Saving the database to the disk" << endl;
  db_save(db, db_path);
  munmap(db->data, sizeof(student_t) * db->psize);
  munmap(db, sizeof(database_t));
  printf("Bye bye!\n");
  return 0;
}
