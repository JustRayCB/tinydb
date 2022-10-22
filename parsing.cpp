#include "parsing.hpp"

#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "student.hpp"

using namespace std;

bool parse_update(char* query, char* field_filter, char* value_filter, char* field_to_update, char* update_value) {
    char* key_val_filter = strtok_r(NULL, " ", &query);  // key=val filter
    if (key_val_filter == NULL) {
        return false;
    }
    if (strtok_r(NULL, " ", &query) == NULL) {  // discard the "set" keyword
        return false;
    }

    char* key_val_update = strtok_r(NULL, " ", &query);  // key=val updated value
    if (key_val_update == NULL) {
        return false;
    }

    if (parse_selectors(key_val_filter, field_filter, value_filter) == 0) {
        return false;
    }
    if (parse_selectors(key_val_update, field_to_update, update_value) == 0) {
        return false;
    }
    return true;
}

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

bool parse_selectors(char* query, char* field, char* value) {
    char* token = strtok_r(NULL, "=", &query); // Premier paramètre = reste du string  qui a été coupé
    if (token == NULL) {
        return false;
    }
    strcpy(field, token);
    token = strtok_r(NULL, "=", &query);
    if (token == NULL) {
        return false;
    }
    strcpy(value, token);
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
    return true;


} 
bool parse_selectors(string &query, int &day, int &mon, int &year){
    stringstream X(query);

    string token;

    if (!getline(X, token, '/')){
        return false;
    }
    day = stoul(token, nullptr, 10);

    if (!getline(X, token, '/')){
        return false;
    }
    mon = stoul(token, nullptr, 10);

    if (!getline(X, token, '/')){
        return false;
    }
    year = stoul(token, nullptr, 10)-1900;

    return true;


}
