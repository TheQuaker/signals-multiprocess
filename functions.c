#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include "workerList.h"


#define FILEBUF 8192

void workOnFile(char* path,char* fileName)
{
	//mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	mode_t mode = 0755;
	int fdread,fdwrite,bytesread,i,c,flag = 0;
	int start = 0;
	int fdot = 0;
	int sdot = 0;
	char buffer[FILEBUF] = {0};
	char array[7] = {0};
	char * domain = NULL;
	char * readFilePath = NULL;
	char * writeFilePath = NULL;
	ListPtr listForDomains = NULL;
	
	//path arxeiwn gia diabasma 
	readFilePath = malloc(strlen(path) + strlen(fileName)+2);
	strcpy(readFilePath, path);
	readFilePath[strlen(path)] = '/';
	strcpy(readFilePath+strlen(path)+1, fileName);
	readFilePath[strlen(path) + strlen(fileName)] = '\0';
	//printf("%s\n",readFilePath);
	if((fdread = open(readFilePath, O_RDONLY, mode)) == -1)
	{
		perror("Failed to open file");
	}
	
	while((bytesread = read(fdread, buffer, FILEBUF)) > 0)
	{
		flag = 0; 
		start = 0;
		fdot = 0;
		sdot = 0;
		if((bytesread == -1))
		{
			perror("Read error");
		}
		//printf("bytesread = %d\n",bytesread);
		for(i=0;i<bytesread;i++)
		{
			if(buffer[i]== 'h' && flag == 0)
			{
				if(strncmp(buffer+i , "http://", 7) == 0)
				{//found valid url
					flag = 1;
					start = i+7;
				}
			}
			else if (flag == 1)
			{
				if(buffer[i] == '.')
				{//find the last dot(s)
					if(fdot == 0)
						fdot = i;
					else if(sdot == 0)
						sdot = i;
					else
					{
						fdot = sdot;
						sdot = i;
					}
				}
				else if((buffer[i] == '/') && (i > start))
				{
					if(sdot == 0)
					{//if only one dot
						sdot = fdot;
						fdot = start-1;
					}
					domain = malloc(i-fdot-1);
					for(c=0; c<i-fdot; c++)
					{
						domain[c] = buffer[fdot+c+1];
					}
					domain[i-fdot-1]= '\0';

					//printf("%s\n",domain);
					insertDomain(&listForDomains, domain);//add domain to list
					flag = 0; 
					start = 0;
					fdot = 0;
					sdot = 0;
					free(domain);
				}
			}
		}
	}
	//path gia dhmiourgeia .out arxeiwn
	writeFilePath = malloc(strlen("outFiles/") + strlen(fileName) + strlen(".out") + 1);
	strcpy(writeFilePath, "outFiles/");
	strcpy(writeFilePath+strlen("outFiles/"), fileName);
	strcpy(writeFilePath + strlen("outFiles/") + strlen(fileName)-1, ".out");
	
	if((fdwrite = open(writeFilePath, O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1)
	{
		perror("Failed to create file for write");
	}
	//grapse sto arxeio ta periexomena ths domhs
	char counter[10] = {0};
	while(!empty(listForDomains))
	{
		write(fdwrite, listForDomains->domain, strlen(listForDomains->domain));
		write(fdwrite, " ", 1);
		sprintf(counter, "%d", listForDomains->value);
		write(fdwrite, counter, strlen(counter));
		write(fdwrite, "\n", 1);
		popFront(&listForDomains);
	}
	
	fileName[strlen(fileName)-1] = '\0';
	printf("The file %s.out was created\n",fileName);
	
	free(readFilePath);
	free(writeFilePath);
	
	close(fdwrite);
	close(fdread);
}
