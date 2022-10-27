#ifndef _QUERY_H
#define _QUERY_H
#include <string>
#include <vector>

#include "db.hpp"

typedef enum { QUERY_SUCCESS, QUERY_FAILURE, UNRECOGNISED_FIELD } QUERY_STATUS;

/**
 * A query_result_t describes the result of a query.
 */
typedef struct {
  student_t* students; /** A list of students **/
  size_t lsize;        /** Logical size **/
  size_t psize;        /** Physical size **/
  QUERY_STATUS status; /** The return status of the query **/
  char query[256];     /** The actual query that was submitted **/
  long start_ns;       /** The start of the query in nanoseconds **/
  long end_ns;         /** The end of the query in nanoseconds **/
} query_result_t;

/**
 * Initialise a query_result_t structure.
 **/
void query_result_init(query_result_t* result, std::string &query);

/**
 * Add a student to a query result.
 **/
void query_result_add(query_result_t* result, student_t s);



//std::vector<student_t*> select(database_t *database, std::string &query);
query_result_t select(database_t *database, std::string query);

int findStudents(database_t *database, const std::string &field, std::string& value, query_result_t &myQuery,
     std::string &v_update, const std::string &f_update = " ");

//Permets d'insert un student dans la base de données s'il n'est pas déjà dedans
query_result_t insert(database_t* database, std::string query);

query_result_t update(database_t *database, std::string query);
void updateStudent(const std::string &f_update, std::string &v_update, student_t &student);

query_result_t deletion(database_t *database, std::string query);
void deleteStudents(database_t *database, std::string field, std::string value, query_result_t &myQuery);

bool getcommand(std::string &select, std::string &update, std::string &insert, std::string &del);
bool getType(std::string &command, std::string &select, std::string &update, std::string &insert, std::string &del);

bool createProcess(pid_t &selectSon, pid_t &updateSon, pid_t &insertSon, pid_t &deleteSon);
#endif
