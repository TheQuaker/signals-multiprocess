#ifndef __HANDLER
#define __HANDLER
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>


void sigchldHandler(int signo);
void sigintHandler(int signo);

#endif
