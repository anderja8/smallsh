/*********************************************************
 *Name: Jacob Anderson
 *Course: CS344
 *Date: 02/08/2020
 *Project: 3, SMALLSH
 *File: backGroundNode.h
 *Description: Implements a linked list to maintain records
 *             on processes running in the background.
 ********************************************************/

#ifndef BACKGROUNDNODE_H
#define BACKGROUNDNODE_H

//Simple linked list struct to maintain a list
//of what processes are running the background
struct backgroundNode {
	pid_t processPid;
	struct backgroundNode* nextProcess;
};

//Function to initialize the struct. Sets the processPID to the passed process
//and the pointer to the next process to NULL.
void initializeBackgroundNode(struct backgroundNode* node, pid_t process);

//Function to add a new node to the linked list with the passed pid.
//Returns a pointer to the newly added node.
struct backgroundNode* addPid(struct backgroundNode** sentinel, struct backgroundNode* currNode, pid_t newPid);

//Function to remove a node with pid value equal to the passed value.
//Returns 1 if found, 0 if not found.
int removePid(struct backgroundNode** sentinel, struct backgroundNode** currNode, pid_t returnedPid);

//Function to correctly set the currNode pointer, called internally by removePid
struct backgroundNode* findCurrNode(struct backgroundNode* sentinel); 

//Function that frees the entire linked list pointed at by the
//passed sentinel pointer. Also sends a term signal to each
//PID in the list and sets sentinel to NULL.
void freeList(struct backgroundNode** sentinel);

//Prints the linked list contents in the form pid1 -> pid2 -> ... -> NULL
//Only used for debugging. Does not appear in smallsh.
void printList(struct backgroundNode* sentinel);

#endif
