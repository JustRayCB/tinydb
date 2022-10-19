#include "student.hpp"
#include <iostream>
#include <fstream>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


void student_to_str(char* buffer, student_t* s) {
  //A fix car cette fonction compare une ligne entiere avec que le first name
  //std::string temp_buffer;
  std::string MyLine = "";
  std::ifstream StudentsFile("students.bin", std::ios::binary);
  while(std::getline(StudentsFile, MyLine)) {
    
    if(strcmp(s->fname, MyLine.c_str())){
      std::string str(buffer);
      strcpy(buffer, MyLine.c_str());
    }
}
  // Your code here
  //short sizeStudent =
      //252; // sizeof(unsigned) + sizeof(char[64])+ sizeof(char[64])
           // +sizeof(char[64]) + sizeof(tm) << endl
  // pas continué
}

int student_equals(student_t* s1, student_t* s2) {
  // Si return 0 alors student_equals et si -1 alors c'est faux
  if (strcmp(s1->fname, s2->fname) and strcmp(s1->lname, s2->lname)){
      return 0;
  }
else{
  return -1;
}
}



