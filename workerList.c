# include "workerList.h"

void addToList(ListPtr* head, int pid)
{//add stopped childs pid
	while(*head != NULL)
	{
		head = &((*head)->next);
	}
	*head = malloc(sizeof(workerList));
	(*head)->value = pid;
	(*head)->domain = NULL;
	(*head)->next = NULL;
}

void popFront(ListPtr* head)
{//remove first element
	ListPtr tmp;
	
	tmp = *head;
	*head = (*head)->next;
	if (tmp->domain != NULL)
		free(tmp->domain);
	free(tmp);
}

int getFirstPid(ListPtr head)
{
	return head->value;
}

int empty(ListPtr head)
{
	if (head == NULL)
		return 1;
	else
		return 0;
}

void insertDomain(ListPtr* head, char* str)
{//add domain to list it's not allready in the list
	int inList = 0;
	
	while(*head != NULL)
	{
		if(strncmp(str, (*head)->domain, strlen(str)) == 0)
		{//if it is in list increase ocurances
			(*head)->value = (*head)->value + 1;
			inList = 1;
			break;
		}
		head = &((*head)->next);
	}
	if(!inList)
	{//add to list
		*head = malloc(sizeof(workerList));
		(*head)->value = 1;
		(*head)->domain = malloc(strlen(str)+1);
		strcpy((*head)->domain, str);
		(*head)->next = NULL;
	}
	
}
/*
void searchForDomain(ListPtr head, char * str)
{
	int inList = 0;
	
	while(head != NULL)
	{
		if(strcmp(head->domain, str) == 0)
		{
			head->value = head->value + 1;
			inList = 1;
			break;
		}
		head = head->next;
	}
	if(!inList)
		insertDomain(&head, str);
}
*/

//for debug
void printList(ListPtr head)
{
	workerList * tmp;
	tmp = head;
	if(!empty(head))
	{
		while(head->next != NULL)
		{
			printf("pid = %d\n",tmp->value);
			tmp = tmp->next;
		}
	}
	else
	{
		printf("Can't print an empty list\n");
	}
}
