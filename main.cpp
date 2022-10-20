#include <stdio.h>
#include <iostream>
#include <time.h>
#include <pthread.h>

#include "db.hpp"
#include "student.hpp"

//void isWorking(database_t *db){
  //for (int idx=0; idx < 10; idx++) {
    //student_t one = db->data[idx];
    //std::cout << "This is the student " << one.fname << " " <<
      //one.lname << " ID: " << one.id << " in section " << one.section << std::endl;


  //}
//}

void isWorking(database_t *db){
  char buffer[512];
  for (int idx=0; idx < 10; idx++) {
    student_t one = db->data[idx];
    student_to_str(buffer, &one);
    std::cout << buffer << std::endl;

  }

}
int main(int argc, char const *argv[]) {
  const char *db_path = argv[argc-1];
  database_t db;
  db_init(&db);
  db_load(&db, db_path);
  isWorking(&db);
  // int count = 0;
  // pthread_t select, insert, del, update ;
  //   pthread_create(&tid2, nullptr, afficher1, &count );
  //   pthread_create(&tid1, nullptr, afficher1, &count );
  //   pthread_create(&tid1, nullptr, afficher1, &count );
  //   pthread_create(&tid1, nullptr, afficher1, &count );
  //   pthread_join(tid1, nullptr); //Attendre de la fin du thread mi en paramètre
  //   pthread_join(tid2, nullptr); //Attendre de la fin du thread mi en paramètre
  // while (count < 10)
  // {
    
  // }
   
  // Il y a sans doute des choses à faire ici...
  db_save(&db, db_path);
  delete [] db.data;
  printf("Bye bye!\n");
  return 0;
}
