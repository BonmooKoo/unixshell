#include "ish.h"


int syntactic_analysis(DynArray_T tokens,DynArray_T commands){
	int num = DynArray_getLength(tokens);
	if (num == 0) {
		fprintf(stderr, "Error: No input provided\n");
		return -1;
	}
	Command* current_command = NULL;
	int in_pipe=0;
	for (int i=0;i<num;i++){
		Token* token = (Token *)DynArray_get(tokens,i);
		// printf("%s\n",token->content);
		if(strcmp(token->content,"|")==0 && token->quoted!=1 ){
			//"|" is not string 
			if(in_pipe){
				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			if(i==num-1){
				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			in_pipe=1;
			if(current_command==NULL){
				fprintf(stderr,"Invalid:Missing command name\n");
				DynArray_free(commands);
				return -1;
			}
			else{
				DynArray_add(commands,current_command);
				current_command=NULL;
			}
		}
		else{
			if(current_command==NULL){
				//First token is function name
				// printf("%s\n",token->content);
				current_command = (Command*)malloc(sizeof(Command));
				strncpy(current_command->command, token->content, strlen(token->content));
				current_command->args=DynArray_new(0);
			}
			else{
				//Following token is arguments
				// printf("%s\n",token->content);
				DynArray_add(current_command->args,token);
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

