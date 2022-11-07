#include "query.hpp"
#include "db.hpp"
#include "parsing.hpp"
#include "student.hpp"
#include "utils.hpp"

#include <bits/iterator_concepts.h>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <sys/mman.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <type_traits>
#include <vector>
#include <time.h>
#include <iostream>
#include <string>

using namespace std;
void query_result_init(query_result_t* result, string& query) {
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  result->start_ns = now.tv_nsec + 1e9 * now.tv_sec;
  result->status = QUERY_SUCCESS;
  result->students = new student_t[10000];
  result->lsize = 0;
  result->psize = 10000;
  strcpy(result->query, query.c_str());
  result->end_ns = 0;
  
}


void query_result_add(query_result_t *result, student_t s){
  
  if (result->lsize == result->psize) {
    student_t* growStudents = new student_t[result->psize+10000];
    for (size_t idx=0; idx < result->psize ; idx++) {
      growStudents[idx] = result->students[idx];
    }
    result->psize += 10000;
    delete [] result->students;
    result->students = growStudents;
  }
  result->students[result->lsize] = s;
  result->lsize++;

}

  
void updateStudent(const string &f_update, string &v_update, student_t &student){
  if (f_update == "id") {
    student.id = (unsigned)stoi(v_update);
  }else if (f_update == "fname") {
    strcpy(student.fname, v_update.c_str());
  }else if (f_update == "lname") {
    strcpy(student.lname, v_update.c_str());
  }else if (f_update == "section") {
    strcpy(student.section, v_update.c_str());
  }else {
    int day, mon, year;
    parse_birthdate(v_update, day, mon, year);
    student.birthdate.tm_mday = day;
    student.birthdate.tm_mon = mon;
    student.birthdate.tm_year = year;
  }


}

int findStudents(database_t *database, const string &field, string& value, query_result_t &myQuery ,
    string &v_update,  const string &f_update){
  size_t dbSize = database->lsize;
  bool isUpdate = false;
  if (f_update != " ") {
    isUpdate = true; // Flag to know if we are into an update command
  }

  for (size_t idx=0; idx < dbSize; idx++) {
    if (field == "id") {
      if (to_string(database->data[idx].id) == value) {
        if (isUpdate) {
          updateStudent(f_update, v_update, database->data[idx]);
          query_result_add(&myQuery, database->data[idx]);
        }else {
          query_result_add(&myQuery, database->data[idx]);
          return 0; // Because the id of a student is unique we juste need to find one
        }
      }
    }else if (field == "fname") {
      if (database->data[idx].fname == value) {
       if (isUpdate) {
          updateStudent(f_update, v_update, database->data[idx]);
        }
        query_result_add(&myQuery, database->data[idx]);
      }
    }else if (field == "lname") {
      if (database->data[idx].lname == value) {
       if (isUpdate) {
          updateStudent(f_update, v_update, database->data[idx]);
        }

        query_result_add(&myQuery, database->data[idx]);
      }
    }else if (field == "section") {
      if (database->data[idx].section == value) {
       if (isUpdate) {
          updateStudent(f_update, v_update, database->data[idx]);
        }
        query_result_add(&myQuery, database->data[idx]);
      }
    }else if (field == "birthdate"){
      int day, mon, year;
      if (parse_birthdate(value, day, mon, year)) {
        if ((day == database->data[idx].birthdate.tm_mday) 
        and (mon == database->data[idx].birthdate.tm_mon) 
        and (year == database->data[idx].birthdate.tm_year)) {
         if (isUpdate) {
          updateStudent(f_update, v_update, database->data[idx]);
          }
          query_result_add(&myQuery, database->data[idx]);
        }
      }else {
        cout << "Problem with the value of birthdate" << endl;
        return 1;
      }
    }else {
      return 2; // field error
    }
  }
  return 0;
}

void swapStudent(database_t *database, size_t &idx, size_t &newSize){
    student_t tmp = database->data[idx];
    database->data[idx] = database->data[newSize-1];
    database->data[newSize-1] = tmp;
    newSize--;
}

int deleteStudents(database_t *database, string field, string value, query_result_t &myQuery) {
  size_t idx=0;
  string toDel = "toDelete"; // Flag to know if we want to delete a student
  size_t newSize = database->lsize;

  while (database->data[idx].section != toDel){ // While we don't reach a deleted student
    if (field == "id") {
      if (to_string(database->data[idx].id) == value) {
        query_result_add(&myQuery, database->data[idx]);
        updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
        break; // id of a student is unique
      }
    }else if (field == "fname") {
      if (database->data[idx].fname == value) {
        query_result_add(&myQuery, database->data[idx]);
        updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
        idx--; // We decrease idx because we have to check if the student we mode at idx will be delete or not
      }
    } else if (field == "lname") {
        if (database->data[idx].lname == value) {
          query_result_add(&myQuery, database->data[idx]);
          updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
        idx--;
      }
    } else if (field == "section") {
        if (database->data[idx].section == value) {
          query_result_add(&myQuery, database->data[idx]);
          updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
        idx--;
      }
    } else if (field == "birthdate") {
        int day, mon, year;
        if (parse_birthdate(value, day, mon, year)) {
          if ((day == database->data[idx].birthdate.tm_mday) 
            and (mon == database->data[idx].birthdate.tm_mon) 
            and (year == database->data[idx].birthdate.tm_year)) {
              query_result_add(&myQuery, database->data[idx]);
              updateStudent("section", toDel, database->data[idx]);
              swapStudent(database, idx, newSize);
              idx--;
          }
        }else {
          cout << "Problem with the value of birthdate" << endl;
          return 1;
        }
      }else {
        return 2;  // Field error
      }
    idx++;
  }
  database->lsize = newSize;
  return 0;

}


query_result_t deletion(database_t *database, string query) {
  query_result_t myQuery;
  string field, value;
  if (!parse_selectors(query, field, value)) {
    cout << "Problem with the query delete" << endl;
    cout << "Please enter the arguments correctly" << endl;
    invalidQuery(myQuery);
    return myQuery;
  }
  string qu = "delete " + query;
  query_result_init(&myQuery, qu);

  int ret =  deleteStudents(database, field, value, myQuery);
  if (ret){
    cout << "Problem with the query delete" << endl;
    if (ret == 2) {
      invalidQuery(myQuery, 2);
    }else{
      invalidQuery(myQuery);
    }
    delete [] myQuery.students;
    return myQuery;
  }

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;

  return myQuery;
}




query_result_t select(database_t *database, string query){
  query_result_t myQuery;
  string field, value;
  if (!parse_selectors(query, field, value)){
    cout << "Problem with the query select" << endl;
    cout << "Please enter the arguments right" << endl;
    invalidQuery(myQuery);
    return myQuery;
  }
  string qu = "select " + query;
  query_result_init(&myQuery, qu);

  int ret = findStudents(database, field, value, myQuery, field);// Last parmateres field will not be used
  if (ret)                                                      //It's here to complete the function's parmateres
  {    
    cout << "Problem with the query select" << endl;
    if (ret == 2) { // If UNRECOGNISED_FIELD
      invalidQuery(myQuery, 2);
    }else {
      invalidQuery(myQuery);
    }
    delete [] myQuery.students;
    return myQuery;

  }

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;


  return myQuery;

}

query_result_t update(database_t *database, string query){
  query_result_t myQuery;
  string field_filter, value_filter, field_to_update, update_value;
  if (!parse_update(query, field_filter, value_filter, field_to_update, update_value)) {
    cout << "Problem with the query update" << endl;
    cout << "Please enter the arguments right" << endl;
    invalidQuery(myQuery);
    return myQuery;

  }
  string qu = "update " + query;
  query_result_init(&myQuery, qu);
  int ret = findStudents(database, field_filter, value_filter, myQuery, update_value, field_to_update);
  if (ret){
    cout << "Problem with the query update" << endl;
    if (ret == 2) {
      invalidQuery(myQuery, 2);
    }else {
      invalidQuery(myQuery);
    }
    delete [] myQuery.students;
    return myQuery;
  }
  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;

  return myQuery;


}

query_result_t insert(database_t *database, std::string query){
  query_result_t myQuery;
  string fname, lname, id, section, birthdate;
  if (!parse_insert(query, fname, lname, id, section, birthdate)) {
    cout << "Problem with the query insert" << endl;
    cout << "Please enter the arguments correctly" << endl;
    invalidQuery(myQuery);
    return myQuery;
  }
  string qu = "insert " + query;
  query_result_init(&myQuery, qu);

  student_t student;
  updateStudent("fname", fname, student);
  updateStudent("lname", lname, student);
  updateStudent("id", id, student);
  updateStudent("section", section, student);

  int day, mon, year;
  if (!parse_birthdate(birthdate, day, mon, year)){
    cout << "Problem with the query insert" << endl;
    cout << "Please enter the arguments correctly" << endl;
    invalidQuery(myQuery);
    return myQuery;
  }
  updateStudent("birthdate", birthdate, student);

  for(size_t i = 0; i < database->lsize;i++){
      if(student.id == database->data[i].id){
        cout<<"Error: Id is already in the database."<<endl;
        invalidQuery(myQuery);
        delete [] myQuery.students;
        return myQuery;
      }
  }

  query_result_add(&myQuery, student); 
  db_add(database, student);

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;
  return myQuery;


}

void invalidQuery(query_result_t &myQuery, unsigned status){
  if (status == 2) {
    myQuery.status = UNRECOGNISED_FIELD;
  }else {
    myQuery.status = QUERY_FAILURE;
  }
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;
}
