#include <csignal>
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

using namespace std;

void isWorking(database_t *db){
  char buffer[512];
  int count = 0;
  for (int idx=0; idx < 10; idx++) {
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
int main(int argc, char const *argv[]) {
  const char *db_path = argv[argc-1];
  database_t db;
  db_load(&db, db_path);

  isWorking(&db);
  pid_t father = getpid();

  //string commandLine; // Va stocker la command de l'usr

  int count = 0 ; // Just pour donner une condition à while;

  // CREATION DES FD:
  int fdSelect[2], fdUpdate[2];//fdDelete[2], fdInsert[2];
  // CREATION PID
  pid_t selectSon = 1, updateSon = 1, deleteSon = 1, insertSon = 1;

  if (pipe(fdSelect) < 0) {perror("Pipes() Select");}
  if (pipe(fdUpdate) < 0) {perror("Pipes() Update");}
  //if (pipe(fdDelete) < 0) {perror("Pipes() Delete");}
  //if (pipe(fdInsert) < 0) {perror("Pipes() Insert");}
  
  if (!createProcess(selectSon, updateSon, insertSon, deleteSon)){ return 1;}
  //// faut utiliser fork
 while (count < 10) {
    if (getpid() == father) {
      //FATHER WRITE
      string selectS = "", updateS = "", insertS = "", delS = "";
      sleep(1);
      getcommand(selectS, updateS, insertS, delS);
      if (selectS != "") {
        char real[256];
        strcpy(real, selectS.c_str());
        write(fdSelect[1], &real, 256);
        //close(fdSelect[1]);
      }else if (updateS != "") {
        char real[256];
        strcpy(real, updateS.c_str());
        write(fdUpdate[1], &real, 256);

      }
      //else if (insertS != "") {
        //char real[256];
        //strcpy(real, insertS.c_str());
        //write(fdInsert[1], &real, 256);
      //}else {
        //char real[256];
        //strcpy(real, delS.c_str());
        //write(fdDelete[1], &real, 256);
      //}

    }
    else {
      //SON READ
      if (selectSon == 0) {
        //cout << "SELECT" << endl;
        char got[256];
        close(fdSelect[1]);
        read(fdSelect[0], &got, 256);
        //cout <<"I have received " <<  got << endl;
        //close(fdSelect[1]);
        string command = got;
        //cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
        query_result_t ret;
        ret = select(&db, command.substr(7, command.length()) );
        log_query(&ret);
        delete [] ret.students;
      }
      else if (updateSon == 0) {
        //cout << "UPDATE" << endl;
        char got[256];
        close(fdUpdate[1]);
        read(fdUpdate[0], &got, 256);
        //cout << "I have received " << got << endl;
        string command = got;
        //cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
        query_result_t ret;
        ret = update(&db, command.substr(7, command.length()) );
        log_query(&ret);
        delete [] ret.students;
      }
      //else if (insertSon == 0) {
        //char got[256];
        //close(fdInsert[1]);
        //read(fdInsert[0], &got, 256);
        //string command = got;
        //query_result_t ret;
        //ret = insert(&db, command.substr(7, command.length()) );
        //log_query(&ret);
        //delete [] ret.students;
      //}else {
        //char got[256];
        //close(fdDelete[1]);
        //read(fdDelete[0], &got, 256);
        //string command = got;
        //query_result_t ret;
        //ret = delete(&db, command.substr(7, command.length()) );
        //log_query(&ret);
        //delete [] ret.students;
      //}
    }
  }  

    // Il y a sans doute des choses à faire ici...
  db_save(&db, db_path);
  delete [] db.data;
  printf("Bye bye!\n");
  return 0;
}
