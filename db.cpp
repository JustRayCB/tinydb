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
    while (fread(&student, sizeof(student_t), 1, file)) {
        db_add(db, student);
    }

    fclose(file);
    std::cout << "Done!" << std::endl;
}

void db_init(database_t *db, size_t &size) {
  db->lsize = 0;                  // Size of the cells that are occupied
  db->psize = 2*size;               // Real size of the list
}

void db_add(database_t *db, student_t student) {
  if (db->lsize <= db->psize) {
    db->data[db->lsize] = student;
    db->lsize++;
  }else {
    std::cout << "There is no more space in the DB, please delete some student" << std::endl;
  }

}

