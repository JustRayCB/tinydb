#include "parsing.hpp"

#include <exception>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <time.h>

#include "student.hpp"

using namespace std;


bool parse_update(string &query, string& field_filter, string &value_filter, string& field_to_update,
        string &update_value){

    stringstream X(query);

    string key_val_filter;
    if (!getline(X, key_val_filter, ' ')) {
        return false;
    }
    string tmp;
    if (!getline(X, tmp, ' ')) {
        return false;
    }
    string key_val_update;
    if (!getline(X, key_val_update, ' ')) {
        return false;
    }

    if (!parse_selectors(key_val_filter, field_filter, value_filter)) {
        return false;
    }

    if (!parse_selectors(key_val_update, field_to_update, update_value)) {
        return false;
    }
    return true;

}


bool parse_insert(char* query, char* fname, char* lname, unsigned* id, char* section, struct tm* birthdate) {
    char* token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(fname, token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(lname, token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    *id = (unsigned)atol(token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(section, token);
    token = strtok_r(NULL, " ", &query);
    if (token == NULL) {
        return false;
    }
    if (strptime(token, "%d/%m/%Y", birthdate) == NULL) {
        return false;
    }
    return true;
}


bool parse_selectors(string &query, string &field, string &value){
    stringstream X(query);

    string token;

    if (!getline(X, token, '=')){
        return false;
    }
    field = token;
    if (!getline(X, token, '=')){
        return false;
    }
 
    value = token;

    if (field == "id") { // If the field is Id check if its an integer
        unsigned tmp;
        try {
        tmp = (unsigned)stoi(value);
        } catch (const invalid_argument& ia) {
            std::cerr << "Invalid argument: " << ia.what() << std::endl;
            return false;
        } catch (const exception& e){
            std::cerr << "Unknown error : " << e.what() << std::endl;
            return false;
        }
        tmp++; // Just not to have the W-Unused warning

    }
    return true;


} 
bool parse_birthdate(string &query, int &day, int &mon, int &year){
    stringstream X(query);

    string token;

    if (!getline(X, token, '/')){
        return false;
    }
    try {
        day = stoul(token, nullptr, 10);
    } catch (const invalid_argument &ia) {
        return false;
    }

    if (!getline(X, token, '/')){
        return false;
    }

    try {
        mon = stoul(token, nullptr, 10) -1; // Parce que c'est décalé de 1 dans tm_mon
    } catch (const invalid_argument &ia) {
        return false;
    }

    if (!getline(X, token, '/')){
        return false;
    }
    try {
        year = stoul(token, nullptr, 10)-1900;
    } catch (const invalid_argument &ia) {
        return false;
    }

    return true;


}
