#include <cstring>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <pthread.h>

#include "db.hpp"
#include "student.hpp"
#include "query.hpp"

using namespace std;

void isWorking(database_t *db){
  char buffer[512];
  int count = 0;
  for (int idx=0; idx < 1000000; idx++) {
    student_t one = db->data[idx];
    if (strcmp(one.section, "info") == 0) {
      //student_to_str(buffer, &one);
      //std::cout << buffer << std::endl;
      count++;
    }
    memset(buffer, 0, sizeof(buffer));
    
  }

  cout << "Le nombre de personnes en info: " << count <<  endl;

}
int main(int argc, char const *argv[]) {
  const char *db_path = argv[argc-1];
  database_t db;
  db_load(&db, db_path);
  isWorking(&db);

  string commandLine; // Va stocker la command de l'usr

  int count = 0 ; // Just pour donner une condition à while;

  //// faut utiliser fork
  while (count < 10)
  {
    cout << "> ";
    getline(std::cin, commandLine);

    string getLine = commandLine.substr(0, 6); // 0->6[ = Size of select, insert, update, delete
    // mettre en lowercase

    if (getLine == "select")
    {
      string test = commandLine.substr(7, commandLine.length());
      select(&db, test);
    }
    count++;

  }
  

    // Il y a sans doute des choses à faire ici...
  db_save(&db, db_path);
  delete [] db.data;
  printf("Bye bye!\n");
  return 0;
}
