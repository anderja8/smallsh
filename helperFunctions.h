/*********************************************************
 *Name: Jacob Anderson
 *Course: CS344
 *Date: 02/08/2020
 *Project: 3, SMALLSH
 *File: helperFunctions.h
 *Description: Implements the helper functions called by
 *             the smallsh. These include:
 *             parseLine, which parses the input line and
 *             assigns strings as appropriate.
 *             readChildStatus, which reads the child
 *             status and determines which exit status
 *             or signal is appropriate.
 ********************************************************/

#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

//Reads the inputLine, and assigns the provided arguments, input, and output files to
//their correct pointers.
void parseLine(char* inputLine, char** arguments, char** inputName, char** outputName, int* isBackground, int* numArgs, int* isComment);

//Reads the child exit status. Sets isSignal to 1 if it was terminated
//by a signal, else 0. Sets status to the int returned by the function
//or the int corresponding to the terminating signal.
void readChildStatus(int childExitStatus, int* status, int* isSignal);

#endif
