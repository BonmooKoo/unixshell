#include "ish.h"
int exec_setenv(char** args){
	// printf("hit im setenv\n");
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
    printf("%s %s\n",var,value);

	if (setenv(var, value, 1) == -1) {
		perror("setenv");
		return -1;
	}
    // printf("get :%s\n",getenv(var));
    // printf("setenv :: %s\n",getenv(var));
	return 0;
}

int exec_unsetenv(char** args){
	// printf("hit im unsetenv\n");
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
	// printf("hit im chdir\n");
    const char *dir;
    if (args[1]){
        dir = args[1];
    }
    else{
		// fprintf(stderr, "HOME\n");
        dir=getenv("HOME");
    }
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
	// printf("hit im exit\n");
    exit(0);
}

int execution(DynArray_T commands){
	fflush(NULL); //clear all IO buffer
    int redirection=0;
    int input_fd=-1; //File descriptor for input redirection "<"" 
    int output_fd=-1; //File descriptor for output redirection ">"
    int pipe_fd=-1;
    int pipeline[2];
    int arg_index=1;
    pid_t pid;
    
    for(int i=0;i<DynArray_getLength(commands);i++){
        arg_index=1;

        //argument set
        Command* cur_command = DynArray_get(commands, i);
        char* cmd = cur_command->command;
        char* args[512];
        args[0]=cmd;
        arg_index=1;

        if(strcmp(cmd,"|")==0){
            // pipe=1;
            continue;
        }
        //ex : cat file1 > file2
        //      0    1   2  3   
        for(int j=0;j<DynArray_getLength(cur_command->args);j++){
            char* arg=DynArray_get(cur_command->args,j);
            // if (strcmp(args[j+1], ">") == 0 || strcmp(args[j+1], "<") == 0) {
            //     redirection=1;
            // }
            if( strcmp(arg, "<") == 0 ){
                //input_redirection
                //ex : cat  <    b
                //    [j] [j+1] [j+2]
                char* input_file=DynArray_get(cur_command->args,j+1);
                // printf("inputfile:%s\n",input_file);
                input_fd=open(input_file,O_RDONLY);
                if(input_fd==-1) {
                    perror(input_file);
                    return -1;
                }
                redirection=1;
                j++;
            }
            else if( strcmp(arg, ">") == 0 ){
                //output_redirection
                //ex : cat  >    b
                //    [j] [j+1] [j+2]
                char* output_file=DynArray_get(cur_command->args,j+1);
                // printf("outputfile:%s\n",output_file);

                output_fd=open(output_file,O_WRONLY|O_CREAT|O_TRUNC,0600);
                if(output_fd==-1) {
                    perror(output_file);
                    return -1;
                }
                redirection=1;
                j++;
            }
            else{
                args[arg_index++]=arg;
            }
        }
        // args[DynArray_getLength(cur_command->args)+1]=NULL; // EOF
        args[arg_index]=NULL;
        

        //run
        if(strcmp(cmd,"setenv")==0){
            if(redirection){
                fprintf(stderr, "Error: Redirection is not allowed with built-in commands\n");
                return -1;
            }
            exec_setenv(args);
        }
        else if(strcmp(cmd,"unsetenv")==0){
            if(redirection){
                fprintf(stderr, "Error: Redirection is not allowed with built-in commands\n");
                return -1;
            }
            exec_unsetenv(args);
        }
        else if(strcmp(cmd,"cd")==0){
            if(redirection){
                fprintf(stderr, "Error: Redirection is not allowed with built-in commands\n");
                return -1;
            }
            exec_chdir(args);
        }
        else if(strcmp(cmd,"exit")==0){
            if(redirection){
                fprintf(stderr, "Error: Redirection is not allowed with built-in commands\n");
                return -1;
            }
            exec_exit(args);
        }
        else{
            //pipeline
            if(i<(DynArray_getLength(commands)-1)){ 
                char* next_arg=DynArray_get(commands, i+1);//struct Command (char*)command
                if(strcmp(next_arg,"|")==0){
                    if(pipe(pipeline)==-1){
                        perror("pipe");
                        return -1;
                    }
                }
            }

            //make child
            pid = fork();
            if (pid == 0) {
                //set signal handler to default
                signal(SIGINT,SIG_DFL);
                signal(SIGQUIT,SIG_DFL);
                // sleep(10);
                
                //redirection : input(>) & pipeline(|)
                if(input_fd!=-1){//set stdinput to fd
                    if(dup2(input_fd,STDIN_FILENO)==-1){
                        perror("dup2 input");
                        exit(EXIT_FAILURE);
                    }
                    close(input_fd);
                }
                else if(pipe_fd != -1){//set stdinput pipeline input
                    dup2(pipe_fd,STDIN_FILENO);
                    close(pipe_fd);
                }
                //redirection : output(<) & pipeline(|)
                if(output_fd!=-1){
                    if(dup2(output_fd,STDOUT_FILENO)==-1){
                        perror("dup2 input");
                        exit(EXIT_FAILURE);
                    }
                    close(output_fd);
                }
                else if(i<(DynArray_getLength(commands)-1)){
                    dup2(pipeline[1],STDOUT_FILENO);
                    close(pipeline[0]);
                    close(pipeline[1]);
                }
                // fprintf(stderr, "cmd : %s\n",args[0]);
                // fprintf(stderr, "args : %s\n",args[1]);
                // fprintf(stderr, "args1 : %s\n",args[2]);
                if(execvp(cmd,args)==-1){
                    perror(cmd);
                    exit(EXIT_FAILURE);
                }
            }
            ///////////////////////
            ////////parent/////////
            ///////////////////////
            else if (pid>0){
                if(input_fd!=-1){
                    close(input_fd);
                }
                if(output_fd!=-1){
                    close(output_fd);
                }
                if(pipe_fd!=-1){
                    close(pipe_fd);
                }
                if(i<(DynArray_getLength(commands)-1)){
                    pipe_fd=pipeline[0];
                    close(pipeline[1]);
                }
                waitpid(pid,NULL,0);
            }
            else{
                perror("fork");
                return -1;
            }
        }
    }
	return 0;
}