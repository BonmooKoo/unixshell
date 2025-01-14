#include "ish.h"
int exec_setenv(char** args){
	printf("hit im setenv\n");
	if (args[1] == NULL) {
		fprintf(stderr, "Error: setenv requires at least a variable name\n");
		return -1;
	}
	const char *var = args[1];
	const char *value;
    if (args[2]){
        value= args[2];
    }
    else{
        value="";
    }
	if (setenv(var, value, 1) == -1) {
		perror("setenv");
		return -1;
	}
    printf("setenv :: %s\n",getenv(var));
	return 0;

}

int exec_unsetenv(char** args){
	printf("hit im unsetenv\n");
    if (args[1] == NULL) {
        fprintf(stderr, "Error: unsetenv requires a variable name\n");
        return -1;
    }
    const char *var = args[1];
    if (unsetenv(var) == -1) {
        perror("unsetenv");
        return -1;
    }
    return 0;

}

int exec_chdir(char** args){
	printf("hit im chdir\n");
	const char *dir = args[1] ? args[1] : getenv("HOME");
	if (dir == NULL) {
		fprintf(stderr, "Error: HOME not set\n");
		return -1;
	}

	if (chdir(dir) == -1) {
		perror("chdir");
		return -1;
	}
	return 0;
}

int exec_exit(char** args){
	printf("hit im exit\n");
    exit(0);
}

int execution(DynArray_T commands){
	fflush(NULL); //clear all IO buffer
    for(int i=0;i<DynArray_getLength(commands);i++){
        //argument set
        Command* cur_command; 
        cur_command = DynArray_get(commands, i);
        char* cmd = cur_command->command;
        pid_t pid;
        char* args[512];
        args[0]=cmd;
        for(int j=0;j<DynArray_getLength(cur_command->args);j++){
            args[j+1]=DynArray_get(cur_command->args,j);
        }
        args[DynArray_getLength(cur_command->args)+1]=NULL; // EOF
        //run
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
            pid = fork();
            if (pid == 0) {
                if(execvp(cmd,args)==-1){
                    perror("execvp");
                    exit(EXIT_FAILURE);
                }
            }
            else{
                waitpid(pid,NULL,0);
            }
        }
    }
	return 0;
}