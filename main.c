#include "ish.h"

int exec_setenv(char* args){
        printf("hit im setenv\n");
}

int exec_unsetenv(char* args){
        printf("hit im unsetenv\n");

}

int exec_chdir(char* args){
        printf("hit im chdir\n");

}

int exec_exit(char* args){
        printf("hit im exit\n");

}

int execution(DynArray_T commands){
        fflush(NULL); //clear all IO buffer
        Command* command; 
        char* cmd;
        pid_t pid[512];
        
        for(int i=0;i<DynArray_getLength(commands);i++){
                command = DynArray_get(commands, i);
                cmd = command->command;
                char* args[512];
                args[0]=cmd;
                for(int j=0;j<DynArray_getLength(command->args);j++){
                        args[j+1]=DynArray_get(command->args,j);
                }
                args[DynArray_getLength(command->args)+1]=NULL;
                
                pid[i] = fork();
                if (pid[i] == 0) {
                        if(strcmp(cmd,"setenv")==0){
                                exec_setenv(args);
                        }
                        else if(strcmp(cmd,"unsetenv")==0){
                                exec_unsetenv(args);
                        }
                        else if(strcmp(cmd,"chdir")==0){
                                exec_chdir(args);
                        }
                        else if(strcmp(cmd,"exit")==0){
                                exec_exit(args);
                        }
                        else{
                                if(execvp(cmd,args)==-1){
                                        pid[i]=0;
                                        // perror("execvp");
                                        // exit(EXIT_FAILURE);
                                }
                        }              
                }
        }
        for(int i=0;i<DynArray_getLength(commands);i++){
                if(pid[i]>0){
                        waitpid(pid[i],NULL,0);
                }
        }
        return 0;
}
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
