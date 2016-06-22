#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "handlers.h"
#include "global.h"
#include "functions.h"

#define FIFO "Npipe"
#define PERMS 0666
#define READ 0
#define WRITE 1
#define BUFSIZE 100

int quitFlag=0;
int addWorker=0;
ListPtr pidList = NULL;

int main(int argc, char* argv[])
{

	pid_t listenerPid = -1;
	pid_t workerPid = -1;
	pid_t tmpPid = -1;
	int fd[2],writefd,readfd;
	int i,c=0,k,fileNameSize,bytes,status,retval = 0;
	
	char fname[BUFSIZE]={0};
	char message[BUFSIZE]={0};
	char * parmList[8] = {"/usr/bin/inotifywait", "-m", "-e", "create", "--format", "%f", NULL, NULL};
	
	static struct sigaction actSigchld;
	static struct sigaction actSigint;
	
	actSigchld.sa_handler = sigchldHandler;
	sigfillset(&(actSigchld.sa_mask));
	sigaction(SIGCHLD, &actSigchld, NULL);
	actSigint.sa_handler = sigintHandler;
	sigfillset(&(actSigint.sa_mask));
	sigaction(SIGINT, &actSigint, NULL);
	
	if(argc == 3)
	{//check for arguments
		if(strcmp(argv[1],"-p") != 0)
		{
			printf("Unknown parameter: %s \n", argv[1]);
			return 1;
		}
		parmList[6] = malloc (strlen(argv[2] + 1));
		strcpy( parmList[6], argv[2]);
		parmList[7] = NULL;
	}
	else if (argc == 1)
	{//set the right path
		parmList[6] = malloc (12);
		strcpy( parmList[6], "../ergasia2");
		parmList[7] = NULL;
	}
	else 
	{
		printf("Wrong number of parameters\n");
		return 1;
	}
	
	if((mkfifo(FIFO, PERMS) < 0) && (errno != EEXIST))
	{//make file for named pipe
		perror("can't create fifo");
		return 1;
	}

	if ( (readfd = open(FIFO, O_RDONLY | O_NONBLOCK)) < 0)
	{
		perror("can't open read fifo");
	}
	if((writefd = open(FIFO, O_WRONLY | O_NONBLOCK)) < 0)
	{
		perror("can't open write fifo");
		//return 1;
	}
	
	if ( pipe(fd) == -1)//pipe for listener
	{
		perror("pipe failed");
		return 1;
	}
	
	listenerPid=fork();
	if (listenerPid == -1)
	{
		perror("Failed to fork");
		return 1;
	}
	
	//this is the child that will exec to inotifywait
	if (listenerPid == 0)
	{
		//redirect stdout to fd[WRITE]
		close( fd[READ]);
		dup2( fd[WRITE], 1);
		close( fd[WRITE]);
		
		if(execvp("/usr/bin/inotifywait", parmList)==-1)
		{
			perror("execvp failed\n");
			printf("errno: %d \n",errno);
			exit(1);
		}
	}
	else//parent (manager)
	{
		close( fd[WRITE]);
		
		while(1)
		{
			fflush(stdout);
			do
			{
				bytes = read(fd[READ], fname, BUFSIZE);
				if(bytes == -1)
				{
					//perror("read error");
				}
				if(quitFlag == 1)
				{
					kill(listenerPid, SIGKILL);
					while(!empty(pidList))
					{
						kill(getFirstPid(pidList), SIGTERM);
						popFront(&pidList);
					}
				}
			}while(bytes < 0);
			if(quitFlag == 1)
				break;

			fflush(stdout);
			for(k=bytes; k < BUFSIZE; k++)
			{
				fname[k] = 0;
			}

			if (write(writefd, fname, BUFSIZE) != BUFSIZE)
			{
				perror("filename write error");
			}
			
			if(empty(pidList))//check for available workers
			{//none available, create one
				workerPid = fork();
				if (workerPid == -1)
				{
					perror("A worker failed to fork");
					return 1;
				}
				else if (workerPid == 0)
				{
					int n,readfdC;
					char fileName[BUFSIZE]={0};
					
					if((readfdC = open(FIFO, O_RDONLY)) < 0)
					{
						perror("can't open read fifo \n");
					}
					
					while(1)
					{
						int i = 0;
						if(n = read(readfdC, fileName, BUFSIZE) < 0)
						{
							perror("worker: data read error");
						}
						while(fileName[i] != 0)
						{	i++;}
						fileName[i] = '\0';
						workOnFile(parmList[6], fileName);
						
						kill(getpid(),SIGSTOP);
					}
				}
			}
			else//sent SIGCONT to the first available
			{
				kill(getFirstPid(pidList), SIGCONT);
				popFront(&pidList);
			}
		}
	}
	
	free(parmList[6]);
	
	close(writefd);
	if(unlink(FIFO) < 0)
	{
		perror("can't unlink \n");
	}
	
	return 0;
}
