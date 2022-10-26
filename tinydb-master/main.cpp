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
  int fdSelect[2], fdUpdate[2], fdDelete[2], fdInsert[2];
  // CREATION PID
  pid_t selectSon = 1, updateSon = 1, deleteSon = 1, insertSon = 1;
  if (!createProcess(selectSon, updateSon, insertSon, deleteSon)){ return 1;}

  if (pipe(fdSelect) < 0) {perror("Pipes() Select");}
  if (pipe(fdUpdate) < 0) {perror("Pipes() Update");}
  if (pipe(fdDelete) < 0) {perror("Pipes() Delete");}
  if (pipe(fdInsert) < 0) {perror("Pipes() Insert");}
  
  //// faut utiliser fork
  while (count < 10)
  {
    string selectS = "", updateS = "", insertS = "", delS = "";
    //if (!selectSon and !updateSon and !insertSon and !deleteSon) { // PARENT PROCESS
    if (getpid() == father) { // PARENT PROCESS
      getcommand(selectS, updateS, insertS, delS);
    }
    if (getpid() == father) {
      kill(selectSon, SIGCONT);
      kill(updateSon, SIGCONT);
      kill(insertSon, SIGCONT);
      kill(deleteSon, SIGCONT);
    }
    //if (selectSon != 1 and updateSon != 1 and insertSon != 1 and deleteSon != 1) { // PARENT PROCESS
    //if (!(selectSon == 0 or updateSon == 0 or insertSon == 0 or deleteSon == 0)) { // PARENT PROCESS
    
    if (selectS != "" and selectSon==0) {
      cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
      query_result_t ret;
      ret = select(&db, selectS.substr(7, selectS.length()) );
      log_query(&ret);
      delete [] ret.students;
      exit(0);

    }
    if (updateS != "" and updateSon==0) {
      cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
      query_result_t ret;
      ret = update(&db, updateS.substr(7, updateS.length()) );
      log_query(&ret);
      delete [] ret.students;
      exit(0);
    }
     if (insertS != "" and insertSon==0) {
      cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
      //query_result_t ret;
      //ret = update(&db, updateS.substr(7, updateS.length()) );
      //log_query(&ret);
      //delete [] ret.students;
      exit(0);
    }
     if ( delS != "" and deleteSon==0) {
      cout << "C'est le process : " << getpid() << " qui fait ça et le père est : " << getppid() << endl;
      //query_result_t ret;
      //ret = update(&db, updateS.substr(7, updateS.length()) );
      //log_query(&ret);
      //delete [] ret.students;
      exit(0);
    }
     if (getpid() != father) {
       exit(0);
     }


    //if (!(selectSon == 0 or updateSon == 0 or insertSon == 0 or deleteSon == 0)) { // PARENT PROCESS
    //if (getpid() == father) { // PARENT PROCESS
      ////kill(selectSon, SIGKILL);
      ////kill(updateSon, SIGKILL);
      ////kill(insertSon, SIGKILL);
      ////kill(deleteSon, SIGKILL);
      //int c;
      //cout << "nb to increment : ";
      //cin >> c;
      //count += c;
    //}
    //waitpid(selectSon, NULL, 0);
    //waitpid(updateSon, NULL, 0);
    //waitpid(insertSon, NULL, 0);
    //waitpid(deleteSon, NULL, 0);
     if (getpid() == father) {
      kill(selectSon, SIGSTOP);
      kill(updateSon, SIGSTOP);
      kill(insertSon, SIGSTOP);
      kill(deleteSon, SIGSTOP);
    }

    count++;

    //cout << "> ";
    //getline(std::cin, commandLine);

    //string getLine = commandLine.substr(0, 6); // 0->6[ = Size of select, insert, update, delete
    // mettre en lowercase

    //if (getLine == "select")
    //{
      //query_result_t ret;
      //string test = commandLine.substr(7, commandLine.length());
      //ret = select(&db, test);
      //log_query(&ret);
      //delete [] ret.students;

    //}else if (getLine == "update") {
      //query_result_t ret;
      //string test = commandLine.substr(7, commandLine.length());
      ////ret = select(&db, test);
      //ret = update(&db, test);
      //log_query(&ret);
      //delete [] ret.students;
    //}
    

  }
  

    // Il y a sans doute des choses à faire ici...
  db_save(&db, db_path);
  delete [] db.data;
  printf("Bye bye!\n");
  return 0;
}
