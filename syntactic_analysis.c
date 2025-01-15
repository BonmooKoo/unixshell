#include "ish.h"


int syntactic_analysis(DynArray_T tokens,DynArray_T commands){
	int num = DynArray_getLength(tokens);
	if (num == 0) {
		fprintf(stderr, "Error: No input provided\n");
		return -1;
	}
	Command* current_command = NULL;
	int in_pipe=0;
	int left_redir=0; // <
	int right_redir=0; // > 
	Token* prev_token=NULL;
	for (int i=0;i<num;i++){
		Token* token = (Token *)DynArray_get(tokens,i);
		// printf("%s\n",token->content);
		if(strcmp(token->content,"|")==0 && token->quoted!=1 ){
			//"|" is not string 
			if(!prev_token || (strcmp(prev_token->content,"|")==0&&prev_token->quoted!=1)){
				// ||
				fprintf(stderr,"test1");
				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			if ((strcmp(prev_token->content,">")==0&&prev_token->quoted!=1)) {
				// > | 
                fprintf(stderr, "Invalid: Standard output redirection without file name\n");
				DynArray_free(commands);
				return -1;
            }
			if (right_redir){
				//cat file1 > out  | grep str
				fprintf(stderr, "Invalid: Multiple redirection of standard out\n");
				DynArray_free(commands);
				return -1;
			}
			if ((strcmp(prev_token->content,"<")==0&&prev_token->quoted!=1)) {
				// < | 
                fprintf(stderr, "Invalid: Standard input redirection without file name\n");
				DynArray_free(commands);
				return -1;
            }
			if(i==num-1){
				// cat |
				fprintf(stderr,"test2");

				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			DynArray_add(commands,current_command);
			prev_token=token;
			current_command=NULL;
			in_pipe=1;
		}
		else if(strcmp(token->content,"<")==0 && token->quoted!=1 ){
			if(left_redir || in_pipe){
				//cat < file1 < file2
				//cat file1 | cat < file2
				fprintf(stderr,"Invalid: Multiple redirection of standard input\n");
				DynArray_free(commands);
				return -1;
			}
			if(!prev_token){
				fprintf(stderr,"test3");

				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			if(i==num-1 || (strcmp(prev_token->content,"|")==0&&prev_token->quoted!=1) ){
				// 1. cat <
				// 2. cat | <
				fprintf(stderr,"Invalid: Standard input redirection without file name\n");
				DynArray_free(commands);
				return -1;
			}
			if(strcmp(prev_token->content,"|")==0 && !prev_token->quoted!=1){
				fprintf(stderr,"Invalid: Standard input redirection without file name\n");
				DynArray_free(commands);
				return -1;
			}
			left_redir=1;
			DynArray_add(commands,current_command);
			prev_token=token;// prev_token = "<"
			current_command=NULL;
		}
		else if(strcmp(token->content,">")==0 && token->quoted!=1 ){
			if(right_redir ){
				// cat file1 > file2 > file3
				// 
				fprintf(stderr,"Invalid: Multiple redirection of standard out\n");
				DynArray_free(commands);
				return -1;
			}
			if(!prev_token){
				fprintf(stderr,"test4");
				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			if(i==num-1 || (strcmp(prev_token->content,"|")==0&&prev_token->quoted!=1) ){
				// 1. cat <
				// 2. cat | <
				fprintf(stderr,"Invalid: Standard output redirection without file name\n");
				DynArray_free(commands);
				return -1;
			}
			if(current_command==NULL){				
				fprintf(stderr,"test5");
				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			right_redir=1;
			DynArray_add(commands,current_command);
			prev_token=token;
			current_command=NULL;
		}
		else{
			if(current_command==NULL){
				//First token is function name
				// printf("%s\n",token->content);
				current_command = (Command*)malloc(sizeof(Command));
				strncpy(current_command->command, token->content, strlen(token->content));
				current_command->args=DynArray_new(0);
				prev_token=token;
			}
			else{
				//Following token is arguments
				// printf("%s\n",token->content);
				DynArray_add(current_command->args,token);
				prev_token=token;
			}
		}
	}

	if (current_command) {//last command
		if (DynArray_getLength(current_command->args) == 0 && !current_command->command) {
			fprintf(stderr, "Invalid: Missing command name\n");
			DynArray_free(commands);
			return -1;
		}
		DynArray_add(commands, current_command);
	}
	/*//test
	printf("Parsed Commands:\n");
	for (int i = 0; i < DynArray_getLength(commands); i++) {
		Command *cmd = DynArray_get(commands, i);
		printf("Command %d: %s\n", i + 1, cmd->command);
		for (int j = 0; j < DynArray_getLength(cmd->args); j++) {
			printf("  Argument: %s\n", (char *)DynArray_get(cmd->args, j));
		}
	}
	*/
}

