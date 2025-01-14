#ifndef ISH_H
#define ISH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>

#include "dynarray.h"

#define MAX_LINE_LENGTH 1023
#define MAX_TOKEN_LENGTH 512 // why? just half of line length
typedef struct{
	//content of token
	char content[MAX_TOKEN_LENGTH];
	//this token is string (it is quoted)
	int quoted;
}Token; 

typedef struct{
	//command
	char command[MAX_TOKEN_LENGTH];
	//arguments
	DynArray_T args;
} Command; 


#endif