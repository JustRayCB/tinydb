#include "student.hpp"
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>


void student_to_str(char* buffer, student_t* s) {
  std::string myBuffer;
  myBuffer = std::string(s->fname) + " " + std::string(s->lname) + "in section " + std::string(s->section)
    + " born on the " + std::to_string(s->birthdate.tm_mday) + "/" + std::to_string(s->birthdate.tm_mon) + "/"
    + std::to_string(s->birthdate.tm_year);
  strcpy(buffer, myBuffer.c_str());


}

int student_equals(student_t* s1, student_t* s2) {
  // Si return 0 alors student_equals et si -1 alors c'est faux
  if (strcmp(s1->fname, s2->fname) and strcmp(s1->lname, s2->lname)){
      return 0;
  }
  return -1;
}



