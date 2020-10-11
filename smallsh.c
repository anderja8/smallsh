/*********************************************************
 *Name: Jacob Anderson
 *Course: CS344
 *Date: 02/08/2020
 *Project: 3, SMALLSH
 *Description: Implements a mini-shell.
 ********************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "backgroundNode.h"
#include "builtInCommands.h"
#include "helperFunctions.h"

//Setting up a global variable to identify whether we are in background mode or not
volatile sig_atomic_t backgroundEnabled = 1;
void handleSIGSTOP(int signo); 

int main() {
	//Initializing the background PID data structure and status/signal flags
	struct backgroundNode* sentinel = NULL;
	struct backgroundNode* currNode = sentinel;
	int stopRunning = 0;
	int childExitStatus;
	int status = 0;
	int isSignal = 0;

	//Set up signal handlers for the shell
	struct sigaction ignoreSig = {{0}};
	struct sigaction dflSig = {{0}};
	struct sigaction handleStp = {{0}};
	ignoreSig.sa_handler = SIG_IGN;
	dflSig.sa_handler = SIG_DFL;
	handleStp.sa_handler = handleSIGSTOP;
	sigfillset(&ignoreSig.sa_mask);
	sigfillset(&dflSig.sa_mask);
	sigfillset(&handleStp.sa_mask);
	ignoreSig.sa_flags = 0;
	dflSig.sa_flags = 0;
	handleStp.sa_flags = 0;
	sigaction(SIGINT, &ignoreSig, NULL);
	sigaction(SIGTSTP, &handleStp, NULL);
	
	//We'll keep this running
	while (!stopRunning) {
		//Check for completed background processes
		pid_t childPid = 0;
		
		if (sentinel != NULL) {
			do {
				childPid = waitpid(-1, &childExitStatus, WNOHANG);
				if (childPid != 0) {
					printf("background pid %u is done: ", childPid);
					fflush(stdout);
					readChildStatus(childExitStatus, &status, &isSignal);
					builtInStatus(status, isSignal);
					removePid(&sentinel, &currNode, childPid);
				}
			} while (childPid != 0 && sentinel != NULL);
		}
		//Printing the prompt and flushing
		printf(": ");
		fflush(stdout);
	
		//Getting the user input, up to 2048 chars allowed
		int readError = 1;
		char* lineEntered = NULL;
		//char lineEntered[2049];
		size_t bufferSize = 0;
		int bytesRead = 0;
		while (readError) {
			bytesRead = getline(&lineEntered, &bufferSize, stdin);

			//If the line is too large, notify the user and reprompt
			if (bytesRead > 2049) {
				printf("Error: too many characters entered into command Must be less than 2048.\n");
			}
			//If the line is greater than 1 char (meaning data was entered) continue
			else if (bytesRead > 1) {
				readError = 0;
			}
			//If we threw the Ctrl+Z interupt while waiting for input, clear error and ask again
			else if (bytesRead == -1) {
				clearerr(stdin);
				printf(": ");
			}
			//Else, flush, reprompt
			else {
				printf(": ");
			}
		}

		//Remove the newline character
		lineEntered[bytesRead - 1] = '\0';

		//Parse the line
		char* arguments[512] = {NULL};
		char* inputName = NULL;
		char* outputName = NULL;
		int isBackground = 0;
		int numArgs = 0;
		int isComment = 0;
		parseLine(lineEntered, arguments, &inputName, &outputName, &isBackground, &numArgs, &isComment);

		//If background mode is disabled, set isBackground to 0
		if (backgroundEnabled == 0) {
			isBackground = 0;
		}

		if (isComment == 0) {
			//If the command is one of the built in commands, cd, status or exit, call it
			if (strcmp(arguments[0], "cd") == 0) {
				builtInCd(arguments[1]);
			}
			else if (strcmp(arguments[0], "status") == 0) {
				builtInStatus(status, isSignal);
			}
			else if (strcmp(arguments[0], "exit") == 0) {
				stopRunning = builtInExit(&sentinel);
			}
			//Otherwise, fork and (try to) run the command
			else {
				int pidFound = -1;
				pid_t spawnpid = fork();
				switch (spawnpid) {
					case -1:
						perror("Error creating fork");
					case 0:
						//Redirecting stdin and stdout where appropriate
						if (outputName != NULL) {
							int outputFD = open(outputName, O_WRONLY | O_CREAT, 0644);
							dup2(outputFD, 1);
						}
						else if (isBackground == 1) {
							int outputFD = open("/dev/null", O_WRONLY | O_CREAT, 0644);
							dup2(outputFD, 1);
						}
						if (inputName != NULL) {
							int inputFD = open(inputName, O_RDONLY, 0644);
							if (inputFD == -1) {
								printf("cannot open %s for input\n", inputName);
								exit(1);
							}
							dup2(inputFD, 0);
						}
						else if (isBackground == 1) {
							int inputFD = open("dev/null", O_RDONLY | O_CREAT, 0644);
							dup2(inputFD, 0);
						}
						//Change signal behavior for the child
						if (isBackground == 0) {
							sigaction(SIGINT, &dflSig, NULL);
						}
						sigaction(SIGTSTP, &ignoreSig, NULL);

						//Calling the exec
						int execStatus = execvp(arguments[0], arguments);

						//Notifying the user if execvp returns with an error and exiting
						if (execStatus == -1) {
							printf("%s: no such file or directory\n", arguments[0]);
							exit(1);
						}
					default:
						//If executed in the foreground, wait for the child to finish
						//This is wrapped in a while loop so that it will continue to
						//wait even if SIGTSTP is sent during execution of a command.
						//Setting SA_RESTART is a more elegent way to handle this, but its
						//behavior with the ": " prompt doesn't match the grading script.
						if (isBackground == 0) {
							while (pidFound == -1) {
								pidFound = waitpid(spawnpid, &childExitStatus, 0);
							}
							readChildStatus(childExitStatus, &status, &isSignal);

							//Immediately notify the user if terminated by a signal
							if (isSignal == 1) {
								builtInStatus(status, isSignal);
							}
						}
						//Otherwise, this is a background process, add to linked list and do
						//not wait
						else {
							printf("background pid is %u\n", spawnpid);
							fflush(stdout);
							currNode = addPid(&sentinel, currNode, spawnpid);
						}
				}
			}
		}
/*
		//Debug printing
		printf("Command is '%s'\n", arguments[0]);
		printf("inputName is '%s'\n", inputName);
		printf("outputName is '%s'\n", outputName);
		printf("isBackground is %u\n", isBackground);
		printf("numArgs is %u\n", numArgs);
		printf("args are: ");
		for (int i=0; i < numArgs; i++) {
			printf("'%s' ", arguments[i]);
		}
		printf("\n");
*/
		//free dynamic memory
		free(lineEntered);
		free(inputName);
		free(outputName);
		for (int i=0; i < numArgs; i++) {
			free(arguments[i]);
		}
	}
	return 0;
}


//Signal handler function for SIGSTOP. Toggles the global variable backgroundEnabled
void handleSIGSTOP(int signo) {
	if (backgroundEnabled > 0) {
		backgroundEnabled = 0;
		char* msg = "Entering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, msg, 50);
	}
	else {
		backgroundEnabled = 1;
		char* msg = "Exiting foreground-only mode\n";
		write(STDOUT_FILENO, msg, 30);
	}
	fflush(stdout);
}
