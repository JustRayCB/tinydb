#include "db.hpp"

#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    FILE *file = fopen(path, "rb");
    if (!file) {
        perror("Could not open the DB file");
        exit(1);
    }
    student_t student;
    while (fread(&student, sizeof(student_t), 1, file)) {
        db_add(db, student);
    }
    fclose(file);
}

void db_init(database_t *db) {
  // Your code here
  db->data = new student_t[100000]; // Initialise la liste pour qu'elle stocke 1000 étudiants
  db->lsize = 0;                  // Taille du nombre de cells occupé
  db->psize = 100000;               // Taille réelle de la liste
}

void db_add(database_t *db, student_t student) {
  // Your code here

  if (db->lsize == db->psize) {
      student_t *growStudents = new student_t[db->psize + 100000];        // Increase the size of the array
      for (size_t idx=0; idx < db->psize; idx++) {
          growStudents[idx] = db->data[idx];    // Copy all the elems of data
      } 
      db->psize = db->psize + 10000;
      
      delete [] db->data;   // Free old memory
      db->data = growStudents;  // Reassign pointer to the expanded array
  }
  db->data[db->lsize] = student;
  db->lsize++;

}
