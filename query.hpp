#ifndef _QUERY_HPP
#define _QUERY_HPP
#include <string>
#include <vector>

#include "db.hpp"

/*
 * query.hpp file for the program tinyDb
 * Groupe: Callens Hugo:000513303, Contuliano Bravo Rayan:000537050, Zaleski Arkadiusz:000516050 
 */


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

/*
 * @brief Function that will manage the select command
 *
 * @param database: pointer to the database struct
 * @param query: Command of the user
 *
 * @return a query_result_t struct with all the result of select command
 */
query_result_t select(database_t *database, std::string query);

/*
 * @brief Function that will find all the student taht match with field=value
 * and update id if it's necessary
 *
 * @param database: pointer to the database
 * @param field: Field we want to search in the database
 * @param value: Value of the field we want to search in the database
 * @param myQuery: pointer to query_result_t struct
 *
 * --------Only with the update function---------------
 * @param v_update: Value we want to the field we need to update
 * @param f_update: Field we want to update
 *
 * @return 0 if the functin was successfull, 1 otherwise
 */
int findStudents(database_t *database, const std::string &field, std::string& value, query_result_t &myQuery,
     std::string &v_update, const std::string &f_update = " ");

/*
 * @brief: Function that will insert a student in a database if he is not
 * already here
 *
 * @param database: pointer to the database
 * @param query: query of the user
 *
 * @return: query_result_t struct with the result
 */
query_result_t insert(database_t* database, std::string query);

/*
 * @brief: Function that will manage the update command from the database
 *
 * @param database: pointer to the database
 * @param query: query of the user
 *
 * @return: query_result_t struct with the result
 */
query_result_t update(database_t *database, std::string query);
/*
 * @brief: Function that will update a student
 *
 * @param f_update: field we want to update
 * @param v_update: value we want to update the field with
 * @param  student: Student we want to update the field with the value
 */
void updateStudent(const std::string &f_update, std::string &v_update, student_t &student);

/*
 * @brief: Function that will manage the delete command
 *
 * @param database: pointer to the database
 * @param query: query of the user
 *
 * @return: query_result_t struct with the result
 */
query_result_t deletion(database_t *database, std::string query);

/* @brief Function that will search the student in a database and then
 * "delete" it
 *
 * @param database: pointer to the database
 * @param field: field we want to delete from the database
 * @param value: value of the field we want to delete from the db
 * @param myQuery: pointer to a query struct
 *
 * @return 0 if the function was successfull, 1 otherwise
 */
int deleteStudents(database_t *database, std::string field, std::string value, query_result_t &myQuery);

/*
 * @brief: Function that will update myQuery with the right stats and the right
 * second when a query fails
 *
 * @param myQuery: pointer to a query_result_t struct
 */
void invalidQuery(query_result_t &myQuery, unsigned status=1);

/* @brief: Function that will swap the student in the idx of the db
 * to the last place in the db and reduce the size of the db of 1e9
 *
 * @param database: pointer to a database 
 * @param idx: database->data[idx] students that we will swap to the database->data[newSize] place
 * @param newSize: Las place in the array of student in the database
 */
void swapStudent(database_t *database, size_t &idx, size_t &newSize);
#endif
