#include "query.hpp"
#include "db.hpp"
#include "parsing.hpp"
#include "student.hpp"
#include "utils.hpp"

#include <cstddef>
#include <iterator>
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
  // Votre code ici
  result->students = new student_t[10000];
  result->lsize = 0;
  result->psize = 10000;
  strcpy(result->query, query.c_str());
  result->end_ns = 0;

  //Pour avoir le temps de fin en nanoseconds
  //struct timespec end;
  //clock_gettime(CLOCK_REALTIME, &now);
  //result->end_ns = end.tv_nsec + 1e9 *end.tv_sec;

  
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
    parse_selectors(v_update, day, mon, year);
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
    isUpdate = true;
  }

  for (size_t idx=0; idx < dbSize; idx++) {

    //Just pour voir les info student en débuggant
    //student_t st = database->data[idx];
    //char one[512];
    //student_to_str(one, &st);
    //cout << one << endl;
    
    if (field == "id") {
      //if (database->data[idx].id == stoul(value, nullptr, 10)) {
      if (database->data[idx].id == (unsigned)stoi(value)) {
        if (isUpdate) {
          updateStudent(f_update, v_update, database->data[idx]);
        }
        query_result_add(&myQuery, database->data[idx]);
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
    }else if (field == "birthday"){
      int day, mon, year;
      if (parse_selectors(value, day, mon, year)) {
        if ((day == database->data[idx].birthdate.tm_mday) 
        and (mon == database->data[idx].birthdate.tm_mon) 
        and (year == database->data[idx].birthdate.tm_year)) {
         if (isUpdate) {
          updateStudent(f_update, v_update, database->data[idx]);
          }

          query_result_add(&myQuery, database->data[idx]);
        }
      }
    }else {
      cout << "ERROR WITH THE FIELD" << endl;
      break;
    }
    //memset(one, 0, sizeof(one)); Pour clear une char array
  }
  return 0;
}

void swapStudent(database_t *database, size_t &idx, size_t &newSize){
    student_t tmp = database->data[idx];
    database->data[idx] = database->data[newSize-1];
    database->data[newSize-1] = tmp;
    newSize--;
}

void deleteStudents(database_t *database, string field, string value, query_result_t &myQuery) {
  size_t idx=0;
  string toDel = "toDelete";
  size_t newSize = database->lsize;

  while (database->data[idx].section != toDel){// or idx < database->lsize) {
    if (field == "id") {
      if (to_string(database->data[idx].id) == value) {
        query_result_add(&myQuery, database->data[idx]);
        updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
        break;
      }
    }else if (field == "fname") {
      if (database->data[idx].fname == value) {
        query_result_add(&myQuery, database->data[idx]);
        updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
      }
    } else if (field == "lname") {
        if (database->data[idx].lname == value) {
          query_result_add(&myQuery, database->data[idx]);
          updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
      }
    } else if (field == "section") {
        if (database->data[idx].section == value) {
          query_result_add(&myQuery, database->data[idx]);
          updateStudent("section", toDel, database->data[idx]);
        swapStudent(database, idx, newSize);
      }
    } else if (field == "birthday") {
        int day, mon, year;
        if (parse_selectors(value, day, mon, year)) {
          if ((day == database->data[idx].birthdate.tm_mday) 
            and (mon == database->data[idx].birthdate.tm_mon) 
            and (year == database->data[idx].birthdate.tm_year)) {
              query_result_add(&myQuery, database->data[idx]);
              updateStudent("section", toDel, database->data[idx]);
              swapStudent(database, idx, newSize);
          }
        }
      }
    //SWAP STUDENT
    //student_t tmp = database->data[idx];
    //database->data[idx] = database->data[newSize-1];
    //database->data[newSize-1] = tmp;
    //idx++;
    //newSize--;
    //if (field == "id") {
      //break;
    //}
    idx++;
  }
  database->lsize = newSize;

  //size_t dbSize = database->lsize;
  //int count = 0;
  //string toDel = "toDelete";
  //for (size_t idx = 0; idx  < dbSize; idx++) {
    //if (field == "id") {
      //if (to_string(database->data[idx].id) == value) {
        //query_result_add(&myQuery, database->data[idx]);
        //updateStudent("section", toDel, database->data[idx]);
        //count++;
      //}
    //} else if (field == "fname") {
      //if (database->data[idx].fname == value) {
        //query_result_add(&myQuery, database->data[idx]);
        //updateStudent("section", toDel, database->data[idx]);
        //count++;
      //}
    //} else if (field == "lname") {
      //if (database->data[idx].lname == value) {
        //query_result_add(&myQuery, database->data[idx]);
        //updateStudent("section", toDel, database->data[idx]);
        //count++;
      //}
    //} else if (field == "section") {
      //if (database->data[idx].section == value) {
        //query_result_add(&myQuery, database->data[idx]);
        //updateStudent("section", toDel, database->data[idx]);
        //count++;
      //}
    //} else if (field == "birthday") {
      //int day, mon, year;
      //if (parse_selectors(value, day, mon, year)) {
        //if ((day == database->data[idx].birthdate.tm_mday) 
        //and (mon == database->data[idx].birthdate.tm_mon) 
        //and (year == database->data[idx].birthdate.tm_year)) {
          //query_result_add(&myQuery, database->data[idx]);
          //updateStudent("section", toDel, database->data[idx]);
          //count++;
        //}
      //}

    //} else {
      //cout << "ERROR WITH FIELD" << endl;
    //}
  //}

  //if (count != 0) {
    // on crée une nouvelle liste d'étudiants de la taille de la précédente moins le compteur
    // (on soustrait le nombre d'étudiants à supprimer)
    //student_t* growStudents = new student_t[database->psize - count];

    //int ignored = 0;

    //for (size_t i = 0; i < database->psize; i++) {
      //if (database->data[i].section != toDel) {
        //growStudents[i - ignored] = database->data[i];
      //} else {
        //ignored++;
      //}
    //}
    //cout << "HERE" << endl;
    //size_t grow = 0;
    //for (size_t idx=0; idx < database->lsize; idx++) {
      //if (database->data[idx].section == toDel) {
      //}else {
        //growStudents[grow] = database->data[idx];
        //grow++;
      //}
    //}

    ////delete []database->data;
    //munmap(database->data, database->psize *(sizeof(student_t)));

    //database->data = growStudents;
    //database->lsize = database->lsize - count;
  //}

}


query_result_t deletion(database_t *database, string query) {
  query_result_t myQuery;
  string field, value;
  if (!parse_selectors(query, field, value)) {
    cout << "Problem with the query delete" << endl;
  }
  string qu = "delete " + query;
  query_result_init(&myQuery, qu);

  cout << "Avant delete " << database->lsize << endl;
  for (int i = 0; i < 10; i++) {
    cout << database->data[i].fname;
  }
  deleteStudents(database, field, value, myQuery);



  cout << "Après delete " << database->lsize << endl;


  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;

  return myQuery;
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



bool getType(std::string &command, std::string &select, std::string &update, std::string &insert, std::string &del){
  if (command.substr(0, 6) == "select") {
    select = command;
  }else if (command.substr(0, 6) == "update") {
    update = command;
  }else if (command.substr(0, 6) == "insert") {
    insert = command;
  }else if (command.substr(0, 6) == "delete"){
    del = command;
  }else {
    return false;
  }
  return true;
}

bool getcommand(std::string &select, std::string &update, std::string &insert, std::string &del, std::string &transaction){
  string commandLine;
  cout << "> ";
  getline(cin, commandLine);
  if (commandLine.substr(0, 11) == "transaction") {
    //commandLine = "";
    //int count = 0;
    //while (commandLine.substr(0, 11) != "transaction" and count < 5) {
      //cout << "> ";
      //getline(cin, commandLine);
      //if (!getType(commandLine, select, update, insert, del)){
        //cout << "Please enter one of these instruction: "
          //"select, update, insert, delete or transaction if you want to end the command " << endl;
        //count ++;
      //}
    //}
    //if (count == 4) {
      //cout << "Sorry you have exceed the max attempt for an instruction" << endl;
      //select = "";
      //update = "";
      //insert = "";
      //del = "";
      //return false;
    //}
    transaction = "transaction";
    return true;
    
  }else if (getType(commandLine, select, update, insert, del)) {
    return true;
  }else {
    cout << "Please enter one of these instruction: "
    "select, update, insert, delete or transaction if you want to end the command " << endl;
    return false;
  }
}


query_result_t select(database_t *database, string query){
  query_result_t myQuery;
  string field, value;
  if (!parse_selectors(query, field, value)) {
    cout << "Problem with the query select" << endl;
  }
  string qu = "select " + query;
  query_result_init(&myQuery, qu);

  findStudents(database, field, value, myQuery,
               field); // Last parmateres field will not be used
                       // It's here to complete the function's parmateres

  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;

  //log_query(&myQuery);
  //delete [] myQuery.students;

  return myQuery;

}

query_result_t update(database_t *database, string query){
  query_result_t myQuery;
  string field_filter, value_filter, field_to_update, update_value;
  if (!parse_update(query, field_filter, value_filter, field_to_update, update_value)) {
    cout << "Problem with the query update" << endl;
  }
  string qu = "update " + query;
  query_result_init(&myQuery, qu);
  
  findStudents(database, field_filter, value_filter, myQuery, update_value, field_to_update);
  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;

  return myQuery;


}

bool createProcess(pid_t &selectSon, pid_t &updateSon, pid_t &insertSon, pid_t &deleteSon){
 
  selectSon = fork();
  if (selectSon < 0) { perror("Fork() Select"); return false;}
  else if (!selectSon) {/*IN SELECT SON PROCESS*/ /*cout << "Select : " << getpid() << " père : " << getppid()<< endl;*/}
  else {/*IN PARENT PROCESS*/
    updateSon = fork();
    if (updateSon < 0) {perror("fork() Update"); return false;}
    else if (!updateSon) {/*IN UPDATE SON PROCESS*/  /*cout << "Update : " << getpid() << " père : " << getppid()<< endl;*/}    
    else { //[>IN PARENT PROCESS<]
      deleteSon = fork();
      if (deleteSon < 0) {perror("fork() Delete"); return false;}
      else if (!deleteSon) {/*[>IN DELETE SON PROCESS<]  cout << "Delete : " << getpid() << " père : " << getppid()<< endl;*/}    
      else {//[>IN PARENT PROCESS<]
        insertSon = fork();
        if (insertSon < 0) {perror("fork() Insert"); return false;}
        else if (!insertSon) {/*[>IN INSERT SON PROCESS<]  cout << "Insert : " << getpid() << " père : " << getppid()<< endl;*/}
      }
    }
  }
  return true;

}

query_result_t insert(database_t* database, string query){
    const char espace = ' ';
    query_result_t myQuery;
    string field_filter, value_filter, field_to_update, update_value;
    //*/if (!parse_i(query, field_filter, value_filter, field_to_update, update_value)) {
       // cout << "Problem with the query insert" << endl;
    //}
    string qu = "insert " + query;
    student_t student;
    query_result_init(&myQuery, qu);
    char* copy_query = strdup(query.c_str());
    strcpy(student.fname, strtok(copy_query, &espace));
    strcpy(student.lname, strtok(NULL, &espace));
    student.id = atoi(strtok(NULL, &espace));
    for(size_t i = 0; i < database->lsize;i++){
      if(student.id == database->data[i].id){
        cout<<"Erreur"<<endl;
        myQuery.status = QUERY_FAILURE;
        free(copy_query);
        return myQuery;
      }
    }
    char* dernier_espace = strtok(NULL, &espace);
    strcpy(student.section, dernier_espace);
    strptime(&copy_query[dernier_espace-copy_query+1],"%e/%m/%Y" ,&student.birthdate);
    if(!parse_insert(copy_query, student.fname, student.lname, &student.id, student.section, &student.birthdate)){
      cout << "Problem with the query insert" << endl;
    }
    myQuery.students[0] = student;
    myQuery.lsize = 1;
    db_add(database, student);
    free(copy_query);

    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    myQuery.end_ns = end.tv_nsec + 1e9 *end.tv_sec;
    return myQuery;
}
