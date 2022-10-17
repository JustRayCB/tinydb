#include <stdio.h>
#include <iostream>

#include "db.hpp"
#include "student.hpp"

void isWorking(database_t *db){
  for (int idx=0; idx < 1000000; idx++) {
    student_t one = db->data[idx];
    std::cout << "This is the student " << one.fname << " " <<
      one.lname << " ID: " << one.id << " in section " << one.section << std::endl;


  }
}
int main(int argc, char const *argv[]) {
  const char *db_path = argv[argc-1];
  database_t db;
  db_load(&db, db_path);
  isWorking(&db);
  
  // Il y a sans doute des choses à faire ici...
  db_save(&db, db_path);
  delete [] db.data;
  printf("Bye bye!\n");
  return 0;
}
