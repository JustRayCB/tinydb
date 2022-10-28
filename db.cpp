#include "db.hpp"

#include <cstddef>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <string>

#include "student.hpp"

void db_save(database_t *db, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) {
        perror("Could not open the DB file");
        exit(1);
    }
    if (fwrite(db->data, sizeof(student_t), db->lsize, f) < 0) {
        perror("Could not write in the DB file");
        exit(1);
    }
    fclose(f);
}

void db_load(database_t *db, const char *path) {
    std::cout << "Loading the database ..." << std::endl;
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;

    // nombre de bits du fichier
    //std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
    //int size = in.tellg()/sizeof(student_t);
    //db_init(db, size);


    //std::cout << "len file : " << in.tellg() << std::endl;

    //std::cout << "len student : " << sizeof(student_t) << std::endl;

    //std::cout << "Preshot nb étudiants : len file / len sutdent = " << in.tellg() / sizeof(student_t) << std::endl;

    while (fread(&student, sizeof(student_t), 1, file)) {
        db_add(db, student);
    }

    fclose(file);
    std::cout << "Done!" << std::endl;
}

void db_init(database_t *db, size_t &size) {
  // Your code here
  //db->data = new student_t[size]; // Initialise la liste pour qu'elle stocke 1000 étudiants
  db->lsize = 0;                  // Taille du nombre de cells occupé
  db->psize = 2*size;               // Taille réelle de la liste
}

void db_add(database_t *db, student_t student) {
  // Your code here

  if (db->lsize == db->psize) {
      student_t *growStudents = new student_t[db->psize + 1000];        // Increase the size of the array
      for (size_t idx=0; idx < db->psize; idx++) {
          growStudents[idx] = db->data[idx];    // Copy all the elems of data
      } 
      db->psize = db->psize + 1000;
      
      delete [] db->data;   // Free old memory
      db->data = growStudents;  // Reassign pointer to the expanded array
  }
  db->data[db->lsize] = student;
  db->lsize++;

}

void dbCopy(student_t *newData, database_t *db){
  std::string toDel = "toDelete";
  size_t grow=0;
  //size_t count=0;
  newData = new student_t[db->psize];
    for (size_t idx=0; idx < db->lsize; idx++) {
      if (db->data[idx].section == toDel) {
      }else {
        newData[grow] =  db->data[idx];
        grow++;
      }
    }

  //for (size_t idx=0; idx < db->lsize; idx++) {
    //if (db->data[idx].section == toDel) {
      //count++;
    //}else{    
    //newData[grow] = db->data[idx];
    //grow++;
    //}
  //}
  //db->lsize = grow;
    //}
}

