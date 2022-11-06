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

/*
 * @brief Function that return the number of students in a binary file
 *
 * @param path Path to the file
 * */
size_t getNumberStudent(const char *path);

/*
 * @brief Function that ask the user to input a command on the terminal and call getType function
 * @param mySelect Instance of Select class
 * @param myUpdate Instance of Update class
 * @param myInsert Instance of Insert class
 * @param myDelete Instance of Delete class
 * @param transaction string that will check if it's a transaction
 *
 * @return true if the command is select update insert delete transaction, false otherwise
 */
bool getCommand(Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete, std::string &transaction);

/*
 * @brief Function that will check the type of the command and udpate the string
 * of the right class
 *
 * @param command usr's input
 * @param mySelect Instance of Select class
 * @param myUpdate Instance of Update class
 * @param myInsert Instance of Insert class
 * @param myDelete Instance of Delete class
 *
 * @return true if it's the right type, false if it's unknown
 */
bool getType(std::string &command, Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete);

/*
 * @brief Function that will Create 4 child process for the father and update
 * the pid of each class. Because One class = One chile process
 *
 * @param mySelect Instance of Select class
 * @param myUpdate Instance of Update class
 * @param myInsert Instance of Insert class
 * @param myDelete Instance of Delete class
 * @return if all the fork function didn't fail
 */
bool createProcess(Select &mySelect, Update &myUpdate, Insert &myInsert, Delete &myDelete);

/*
 * @brief Function that will check if a file exists or not
 *
 * @param dbPath Path to the database's file
 *
 * @return true if file exists, false otherwise
 */
int fileExist(const string dbPath);
#endif  // _UTILS_H
