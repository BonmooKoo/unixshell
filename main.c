#include "ish.h"

int execute_line(const char* line){
	// printf("%s\n",line);
	DynArray_T tokens = DynArray_new(0);//token array for token
	DynArray_T commands = DynArray_new(0);//dynamic array for command
	int lex=lexical_analysis(line,tokens);
	if (lex == -1 ){
		return -1;
	}
	int syn=syntactic_analysis(tokens,commands);
	if (syn == -1 ){
		return -1;
	}
	int exec=execution(commands);
	DynArray_free(tokens);
	DynArray_free(commands);
}

void read_ishrc(){
	//1. READ ishrc file
	char *home_dir = getenv("HOME");
	char ishrc_path[MAX_LINE_LENGTH];
	// snprintf(ishrc_path,sizeof(ishrc_path),"%s/sampleish",home_dir);
	snprintf(ishrc_path,sizeof(ishrc_path),"%s/.ishrc",home_dir);
	// printf("%s\n",ishrc_path);
	FILE *file = fopen(ishrc_path,"r");
	if (!file) {
		// printf("return\n");
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
	char line[MAX_LINE_LENGTH+1];
	char *home_dir = getenv("HOME");
	set_sighandler();
	// read_ishrc();
	fflush(NULL); //clear all IO buffer

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
