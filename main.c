#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#define MAX_LINE_LENGTH 1023

void execute_line(char line){
        printf("%s\n",line);
}

void read_ishrc(){
        //1. READ ishrc file
        char *home_dir = getenv("HOME");
        char ishrc_path[MAX_LINE_LENGTH];
        snprintf(ishrc_path,sizeof(ishrc_path),"%s/.ishrc",home_dir);
        FILE *file = fopen(ishrc_path,"r");
        if (!file) {
                return;
        }
        char line[MAX_LINE_LENGTH+1];
        while(fgets(line,sizeof(line),file)){
                printf("%% %s",line);
                execute_line(line);
        }
        fclose(file);
}
int main(void){
        //print a prompt
        read_ishrc();
	char line[MAX_LINE_LENGTH+1];
        char *home_dir = getenv("HOME");
        while (1) {
                printf("%% ");
                if (!fgets(line, sizeof(line), stdin)) {
                	printf("\n");
                	break;
                }
                execute_line(line);
        }
        return 0;
}      
