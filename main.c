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
#define MAX_TOKEN_LENGTH 512 // why? just half of line length
typedef struct{
	//content of token
	char content[MAX_TOKEN_LENGTH];
	//this token is string (it is quoted)
	int quoted;
}Token; 

uint error_handling(){

}
int add_token(DynArray_T tokens, const char *buffer, int quoted) {
    Token *token = malloc(sizeof(Token));
    if (!token) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return -1;
    }
    strncpy(token->content, buffer, MAX_TOKEN_LENGTH);
    token->quoted = quoted;
    DynArray_add(tokens, token);
    return 0;
}

int lexical_analysis(const char *line, DynArray_T tokens) {
    printf("%s\n",line);
    const char *ptr = line;
    char buffer[MAX_TOKEN_LENGTH];
    int index = 0;
    int is_quote = 0;
    char quote_char = '\0';

    while (*ptr != '\0') {
        if (is_quote) {
            if (*ptr == quote_char) {
                // buffer[index] = '\0';
                // if (add_token(tokens, buffer, 1) == -1) {
                //     return -1;
                // }
                // index = 0;
                is_quote = 0;
            } else {
                buffer[index++] = *ptr;
                if (index >= MAX_TOKEN_LENGTH) {
                    fprintf(stderr, "Error: Token too long\n");
                    return -1;
                }
            }
        } else {
            if (*ptr == '"' || *ptr == '\'') {
                is_quote = 1;
                quote_char = *ptr;
                // if (index > 0) {
                //     buffer[index] = '\0';
                //     if (add_token(tokens, buffer, 0) == -1) {
                //         return -1;
                //     }
                //     index = 0;
                // }
            } else if (isspace(*ptr)) {
                if (index > 0) {
                    buffer[index] = '\0';
                    if (add_token(tokens, buffer, 0) == -1) {
                        return -1;
                    }
                    index = 0;
                }
            } else if (*ptr == '|' || *ptr == '<' || *ptr == '>') {
                if (index > 0) {
                    buffer[index] = '\0';
                    if (add_token(tokens, buffer, 0) == -1) {
                        return -1;
                    }
                    index = 0;
                }
                buffer[0] = *ptr;
                buffer[1] = '\0';
                if (add_token(tokens, buffer, 0) == -1) {
                    return -1;
                }
            } else {
                buffer[index++] = *ptr;
                if (index >= MAX_TOKEN_LENGTH) {
                    fprintf(stderr, "Error: Token too long\n");
                    return -1;
                }
            }
        }
        ptr++;
    }

    if (is_quote) {
        fprintf(stderr, "Error: Unmatched quote\n");
        return -1;
    }

    if (index > 0) {
        buffer[index] = '\0';
        if (add_token(tokens, buffer, 0) == -1) {
            return -1;
        }
    }

//     /*test
    printf("len : %d\n",DynArray_getLength(tokens));
    for(int i=0;i<DynArray_getLength(tokens);i++){
        Token *token = (Token *)DynArray_get(tokens, i);
        printf("test : %s\n",token->content);
    }
//     */
    return 0;
}

int execute_line(const char* line){
        // printf("%s\n",line);
        DynArray_T tokens = DynArray_new(0);//token array for token
	int lex=lexical_analysis(line,tokens);
        if (lex == -1 ){
                return -1;
        }	
}

void read_ishrc(){
        //1. READ ishrc file
        char *home_dir = getenv("HOME");
        char ishrc_path[MAX_LINE_LENGTH];
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
        // read_ishrc();
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
