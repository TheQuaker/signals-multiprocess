# include <stdlib.h>
# include <string.h>
# include <stdio.h>

typedef struct listNode *ListPtr;

typedef struct listNode {
	int value;
	char * domain;
	struct listNode * next;
}workerList;


void addToList(ListPtr* head, int value);
void popFront(ListPtr* head);
int getFirstPid(ListPtr head);
int empty(ListPtr head);
void insertDomain(ListPtr* head, char* str);
//void searchForDomain(ListPtr head, char * str);
void printList(ListPtr head);
