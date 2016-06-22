#include "handlers.h"
#include "global.h"

void sigchldHandler(int signo)
{
	int tmpPid, status;
	if (signo == SIGCHLD)
	{
		tmpPid = waitpid(-1, &status, WUNTRACED | WCONTINUED);
		if (WIFSTOPPED(status))
		{
			addToList(&pidList, tmpPid);
		}
	}
}

void sigintHandler(int signo)
{
	if (signo == SIGINT)
	{
		quitFlag = 1;
	}
}
