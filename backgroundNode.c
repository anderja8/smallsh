/*********************************************************
 *Name: Jacob Anderson
 *Course: CS344
 *Date: 02/08/2020
 *Project: 3, SMALLSH
 *File: backGroundNode.c
 *Description: Implements a linked list to maintain records
 *             on processes running in the background.
 ********************************************************/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "backgroundNode.h"

//Function to initialize the struct. Sets the processPID to the passed process
//and the pointer to the next process to NULL.
void initializeBackgroundNode(struct backgroundNode* node, pid_t process) {
	node->processPid = process;
	node->nextProcess = NULL;
}

//Function to add a new node to the linked list with the passed pid.
//Returns a pointer to the newly added node.
struct backgroundNode* addPid(struct backgroundNode** sentinel, struct backgroundNode* currNode, pid_t newPid) {
	struct backgroundNode* tempNode = malloc(sizeof(struct backgroundNode));
	initializeBackgroundNode(tempNode, newPid);
	if (currNode == NULL) {
		*sentinel = tempNode;
		return tempNode;
	}
	else {
		currNode->nextProcess = tempNode;
		return tempNode;
	}
}

//Function to remove a node with pid value equal to the passed value.
//Ensure sentinel and currNode pointers remain accurate.
//Returns 1 if found, 0 if not found.
int removePid(struct backgroundNode** sentinel, struct backgroundNode** currNode, pid_t returnedPid) {
	//First, if the sentinel is NULL, we won't find the pid, return
	if (sentinel == NULL) {
		*currNode = findCurrNode(*sentinel);
		return 0;
	}
	//Else, if the sentinel is the pid, we're looking for, return it
	else if ((*sentinel)->processPid == returnedPid) {
		struct backgroundNode* temp = (*sentinel)->nextProcess;
		free(*sentinel);
		*sentinel = temp;
		*currNode = findCurrNode(*sentinel);
		return 1;
	}
	//Otherwise, we'll need to traverse the list until the pid is found
	else {
		//Because the linked list only has next, I'll need to maintain two pointers
		//to iterate over the list
		struct backgroundNode* tempPrev = *sentinel;
		struct backgroundNode* temp;
		
		do {
			//First move temp forward from temp prev
			temp = tempPrev->nextProcess;
			//Now checking if temp is NULL
			if (temp == NULL) {
				*currNode = findCurrNode(*sentinel);
				return 0;
			}
			//Now checking if temp is the desired pid
			else if (temp->processPid == returnedPid) {
					tempPrev->nextProcess = temp->nextProcess;
					free(temp);
					*currNode = findCurrNode(*sentinel);
					return 1;
			}	
			//If not, continue to increment
			else {
				tempPrev = tempPrev->nextProcess;
			}
		} while (tempPrev != NULL);
	}
	//If we iterate over the whole list and don't find the pid, return 0. Do not reassign currNode	
	return 0;
}

//Function to correctly set the currNode pointer, called internally by removePid
struct backgroundNode* findCurrNode(struct backgroundNode* sentinel) {
	if (sentinel == NULL) {
		return NULL;
	}
	else {
		struct backgroundNode* temp = sentinel;
		while (1) {
			if (temp->nextProcess != NULL) {
				temp = temp->nextProcess;
			}
			else {
				return temp;
			}
		}
	}
}

//Function that frees the entire linked list pointed at by the
//passed sentinel pointer.
void freeList(struct backgroundNode** sentinel) {
	while (*sentinel != NULL) {
		struct backgroundNode* temp = *sentinel;
		*sentinel = (*sentinel)->nextProcess;
		kill(temp->processPid, SIGTERM);
		free(temp);
	}
}

//Prints the linked list contents in the form pid1 -> pid2 -> ... -> NULL
//Only used for debugging. Does not appear in smallsh.
void printList(struct backgroundNode* sentinel) {
	struct backgroundNode* temp = sentinel;
	do {
		if (temp == NULL) {
			printf("NULL");
		}		
		else {
			printf("%u -> ", temp->processPid);
			temp = temp->nextProcess;
		}
	} while (temp != NULL);
	printf("\n");
}
