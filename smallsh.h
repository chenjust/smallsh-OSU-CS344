/*****************************
 *Justin Chen
 *smallsh.h
 *Header file containing function headers
******************************/
#ifndef SMALLSH_H
#define SMALLSH_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/wait.h>

//Function headers
int pid(char*);
int Space(char*, int);
int inputOutput(int, char*);
int inputLen(char*);
void signalControlC(int);
void signalControlZ(int);
void argument(char**, char*);
void files(char*, int , int);

#endif