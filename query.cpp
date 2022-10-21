#include "query.hpp"
#include "db.hpp"
#include "parsing.hpp"
#include "student.hpp"

#include <algorithm>
#include <cstring>
#include <iterator>
#include <streambuf>
#include <vector>
#include <time.h>
#include <iostream>
#include <string>

using namespace std;
void query_result_init(query_result_t* result, const char* query) {
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->status = QUERY_SUCCESS;
  // Votre code ici
  result->students = new student_t[1000];
  result->lsize = 0;
  result->psize = 1000;
  strcpy(result->query, query);
  result->end_ns = 0;

  //Pour avoir le temps de fin en nanoseconds
  //struct timespec end;
  //clock_gettime(CLOCK_REALTIME, &now);
  //result->end_ns = end.tv_nsec + 1e9 *end.tv_sec;

  
}


void findStudents(database_t *database, string &field, string& value, vector<student_t*> &myStudents){
  size_t dbSize = database->lsize;

  for (size_t idx=0; idx < dbSize; idx++) {

    //Just pour voir les info student en débuggant
    //student_t st = database->data[idx];
    //char one[512];
    //student_to_str(one, &st);
    //cout << one << endl;
    
    if (field == "id") {
      if (database->data[idx].id == stoul(value, nullptr, 10)) {
        myStudents.push_back(&(database->data[idx]));
      }
    }else if (field == "fname") {
      if (database->data[idx].fname == value) {
        myStudents.push_back(&(database->data[idx]));
      }
    }else if (field == "lname") {
      if (database->data[idx].lname == value) {
        myStudents.push_back(&(database->data[idx]));
      }
    }else if (field == "section") {
      if (database->data[idx].section == value) {
        myStudents.push_back(&(database->data[idx]));
      }
    }else if (field == "birthday"){
      int day, mon, year;
      if (parse_selectors(value, day, mon, year)) {
        if ((day == database->data[idx].birthdate.tm_mday) 
        and (mon == database->data[idx].birthdate.tm_mon) 
        and (year == database->data[idx].birthdate.tm_year)) {
          myStudents.push_back(&(database->data[idx]));
        }else {
          cout << "ERROR WITH BIRTHDAY" << endl;
        }
      }
    }else {
      cout << "ERROR WITH THE FIELD" << endl;
    }
    //memset(one, 0, sizeof(one)); Pour clear une char array
}


}

// FONCTION SELECT
// DOIT LA TRANSFOMER POUR QU'ELLE RENVOIE UN QUERY_RESULT_T
//vector<student_t*> select(database_t *database, string &query){
  //string field, value;
  //if (!parse_selectors(query, field, value)) {
    //cout << "Problem with the query" << endl;
  //}

  //vector<student_t*> studentList;
  //findStudents(database, field, value, studentList);
  //char buffer[512];
  //for (auto student : studentList) {
    
    //student_to_str(buffer, student);
    //memset(buffer, 0, sizeof(buffer));
  //}
  //cout << "Taille liste :" << studentList.size() << endl;

  //return studentList;
//}


query_result_t select(database_t *database, string &query){

  query_result_t myQuery;
  string field, value;
  if (!parse_selectors(query, field, value)) {
    cout << "Problem with the query" << endl;
  }
  query_result_init(myQuery, const char *query);


}
