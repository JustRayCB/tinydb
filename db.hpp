#ifndef _DB_H
#define _DB_H

#include "student.hpp"
#include <cstddef>

/*
 * db.hpp file for the program tinyDb
 * Groupe: Callens Hugo:000513303, Contuliano Bravo Rayan:000537050, Zaleski Arkadiusz:000516050 
 */



/**
 * Database structure type.
 */
typedef struct {
  student_t *data; /** The list of students **/
  size_t lsize;    /** The logical size of the list **/ // Nombre d'éléments dans la liste size()
  size_t psize;    /** The physical size of the list **/ // Taille de la liste en mémoire sizeof()
} database_t;

/**
 *  Add a student to the database.
 **/
void db_add(database_t *db, student_t s);

/**
 * Save the content of a database_t in the specified file.
 **/
void db_save(database_t *db, const char *path);

/**
 * Load the content of a database of students.
 **/
void db_load(database_t *db, const char *path);

/**
 * Initialise a database_t structure.
 * Typical use:
 * ```
 * database_t db;
 * db_init(&db);
 * ```
 **/
void db_init(database_t *db, size_t &size);

#endif
