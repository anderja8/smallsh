/*********************************************************
 *Name: Jacob Anderson
 *Course: CS344
 *Date: 02/08/2020
 *Project: 3, SMALLSH
 *File: helperFunctions.c
 *Description: Implements the helper functions called by
 *             the smallsh. These include:
 *             parseLine, which parses the input line and
 *             assigns strings as appropriate.
 *             readChildStatus, which reads the child
 *             status and determines which exit status
 *             or signal is appropriate.
 ********************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "helperFunctions.h"

//Reads the inputLine, and assigns the provided arguments, input, and output files to
//their correct pointers.
void parseLine(char* inputLine, char** arguments, char** inputName, char** outputName, int* isBackground, int* numArgs, int* isComment) {	
	*numArgs = 0;

	//First, check if this is a comment line
	if (inputLine[0] == '#') {
		*isComment = 1;
		return;
	}

	//Despite the dire lecture warnings, I'm going to use strtok to break apart the string
	const char* space = " ";
	char* token = strtok(inputLine, space);
	
	//Keep parsing the string until token returns NULL
	while (token) {

		//Determine the number of characters to allocate for the token
		//If the string contains '$$', I will allocate an extra 4 characters (pid can be up to 5 chars + nt)
		//If the string does not cotain '$$', then I will only allocate 1 extra character (for the nt)
		int charBuf = 0;
		int needsExpand = 0;
		pid_t currPid = getpid();
		char pidString[6];
		memset(pidString, '\0', 6);
		snprintf(pidString, 6, "%u", currPid);
		if (strstr(token, "$$")) {
			charBuf = strlen(token) + 4;
			needsExpand = 1;
		}
		else {
			charBuf = strlen(token) + 1;
		}

		//Handling input files
		if (strstr(token, "<")) {
			token = strtok(NULL, space);
			*inputName = malloc(charBuf*sizeof(char));
			memset(*inputName, '\0', charBuf);
			strcpy(*inputName, token);
			if (needsExpand == 1) {
				char* startOfExp = strstr(*inputName,"$$");
				strcpy(startOfExp,pidString);
			}
			*isBackground = 0;
		}
		//Handling output files
		else if (strstr(token, ">")) {
			token = strtok(NULL, space);
			*outputName = malloc(charBuf*sizeof(char));
			memset(*outputName, '\0', charBuf);
			strcpy(*outputName, token);
			if (needsExpand == 1) {
				char* startOfExp = strstr(*outputName,"$$");
				strcpy(startOfExp,pidString);
			}
			isBackground = 0;
		}
		//Handling background processes
		//Because the other handlers set isBackground to 0, this will only
		//persist if the "&" character is the last command sent to the shell
		//prompted (which is the expected behavior.
		else if (strcmp(token, "&") == 0) {
			token = strtok(NULL, space);
			*isBackground = 1;
		}
		//Treating everything else as an argument
		else {
			arguments[*numArgs] = malloc(charBuf*sizeof(char));
			memset(arguments[*numArgs], '\0', charBuf);
			strcpy(arguments[*numArgs], token);
			if (needsExpand == 1) {
				char* startOfExp = strstr(arguments[*numArgs],"$$");
				strcpy(startOfExp,pidString);
			}
			*numArgs = *numArgs + 1;
			*isBackground = 0;
		}
		token = strtok(NULL, space);
	}
}

//Reads the child exit status. Sets isSignal to 1 if it was terminated
//by a signal, else 0. Sets status to the int returned by the function
//or the int corresponding to the terminating signal.
void readChildStatus(int childExitStatus, int* status, int* isSignal) {
	//First, check if the process exited with a signal
	if (WIFSIGNALED(childExitStatus) != 0) {
		*isSignal = 1;
		*status = WTERMSIG(childExitStatus);
	}
	else {
		*isSignal = 0;
		*status = WEXITSTATUS(childExitStatus);
	}
	return;
}
