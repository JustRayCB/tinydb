#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include "query.hpp"
#include "select.hpp"
#include "update.hpp"
#include "delete.hpp"
#include "insert.hpp"

/**
 * @brief Effectue un `read` en vérifiant la valeur de retour.
 * Si la valeur de retour est négative, la fonction affiche l'erreur et termine
 * le processus avec `exit(1)`.
 *
 * @param fd le file descriptor depuis lequel lire
 * @param buffer le buffer dans lequel mettre les données lues
 * @param nbytes le nombre de bytes maximum à lire
 * @return size_t le nombre de bytes lus
 */
size_t safe_read(int fd, void *buffer, size_t nbytes);

/**
 * @brief Effectue un `write` en vérifiant la valeur de retour.
 * Si la valeur de retour est négative, la fonction affiche l'erreur et termine
 * le processus avec `exit(1)`.
 *
 * @param fd le file descriptor dans lequel écrire
 * @param buffer le buffer dans lequel se trouvent les données
 * @param nbytes le nombre de bytes à écrire
 * @return size_t le nombre de bytes écrits
 */
size_t safe_write(int fd, const void *buffer, size_t nbytes);

/**
 * @brief the result of a query.
 **/
void log_query(query_result_t *result);

size_t getNumberStudent(const char *path);

bool getCommand(Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete, std::string &transaction);
bool getType(std::string &command, Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete);

bool createProcess(Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete);
#endif  // _UTILS_H
