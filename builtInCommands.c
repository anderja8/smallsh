/*********************************************************
 *Name: Jacob Anderson
 *Course: CS344
 *Date: 02/08/2020
 *Project: 3, SMALLSH
 *File: builtInCommands.c
 *Description: Implements the three built in shell functions:
 *             cd, exit, and status.
 ********************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "backgroundNode.h"

//Function to implement the built in cd functionality. Takes a char* that is the directory
//and attempts to change the working directory to this. If NULL, changes the directory to HOME.
//Returns the value of the chdir function.
int builtInCd(char* path) {
	int retVal;
	if (path == NULL) {
		retVal = chdir(getenv("HOME"));
	}
	else {
		retVal = chdir(path);
	}
	return retVal;
}

//Function to implement the built in status functionality. Simply prints the passed status int
//and flushes the stdout buffer. The boolean int, isSignal is used to determine whether to print
//as if this is a signal or exit status.
void builtInStatus(int status, int isSignal) {
	if (isSignal == 0) {
		printf("exit value %u\n", status);
	}
	else {
		printf("terminated by signal %u\n", status);
	}
	fflush(stdout);
}

//Function to implement the exit functionality. First kills any background processes, and then
//returns a 1.
int builtInExit(struct backgroundNode** sentinel) {
	freeList(sentinel);
	return 1;
}
