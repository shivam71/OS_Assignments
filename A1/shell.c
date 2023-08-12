#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>

void command_ls(){
    char *args[2];
    args[0]=strdup("ls");
    args[1]=NULL;
    execvp(args[0],args);
    return;
}

void command_help(){
    printf("Hi, I am here to help. Here is a list of commands that I support:\n");
    printf(">ls\n>help\n>cd\n>mkdir\n");
    return;
}

char* parse_input(char* user_input){
    printf("\nMTL458>");
    fflush(stdout);
    
    fgets(user_input,sizeof(user_input),stdin);
    user_input[strcspn(user_input,"\n")]='\0';
    
    char *command = strtok(user_input," ");
    return command;
}

void command_switch(char* command){
    int pid = fork();
    if(pid<0){
        printf("Error couldn't create a child process try again !\n");
    }else if(pid==0){
        // child process got created
        if(strcmp("ls",command)==0){
            command_ls();
        }else if(strcmp("help",command)==0){
            command_help();
        }else{
            printf("Command not supported\n");
        }
    }else{
        int wc = wait(NULL);
    }
}

int main(){
	while(1){
		char user_input[1000];
        char *command = parse_input(user_input);
        command_switch(command);
		// String length problem if input is less than hundered characters ?
		printf("User input was %s",user_input);// Note that the user input has the newline character 
	}


	return 0;
}
