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

#include "dynarray.h"
#define MAX_LINE_LENGTH 1023
typedef struct{
	//content of token
	char content[512];
	//this token is string (it is quoted)
	int isString;
}Token; 

uint error_handling(){

}

int lexical_analysis(const char* line,DynArray_T tokens){
	printf("lexical_anal : %s\n",line);
        if()        

}
int execute_line(const char* line){
        printf("%s\n",line);
        DynArray_T tokens = DynArray_new(512);//token array for token
	int lex=lexical_analysis(line,tokens);
        if (lex == -1 ){
                return -1;
        }	
}

void read_ishrc(){
        //1. READ ishrc file
        char *home_dir = getenv("HOME");
        char ishrc_path[MAX_LINE_LENGTH];X
	snprintf(ishrc_path,sizeof(ishrc_path),"%s/sampleish",home_dir);
        //snprintf(ishrc_path,sizeof(ishrc_path),"%s/.ishrc",home_dir);
        printf("%s\n",ishrc_path);
	FILE *file = fopen(ishrc_path,"r");
        if (!file) {
                printf("return\n");
		return;
        }
        char line[MAX_LINE_LENGTH+1];
        while(fgets(line,sizeof(line),file)){
                printf("%% %s",line);
                line[strcspn(line, "\n")] = '\0';
		if(execute_line(line)==-1){
                        //there is error reading line

                }
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
                line[strcspn(line, "\n")] = '\0';
		execute_line(line);
        }
        return 0;
}      
