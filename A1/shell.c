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


int main(){
	while(1){
		char user_input[100];
		printf("\nMTL458>");
		fflush(stdout);
		fgets(user_input,sizeof(user_input),stdin);
		user_input[strcspn(user_input,"\n")]='\0';
		int pid = fork();
		if(pid<0){
			printf("Error couldn't create a child process try again !\n");
		}else if(pid==0){
			// child process got created
			char *command = strtok(user_input," ");
			if(strcmp("ls",command)==0){
                command_ls();
			}else{
				printf("Command not supported\n");
			}
		}else{
			int wc = wait(NULL);
		}
		
		// String length problem if input is less than hundered characters ?
		printf("User input was %s",user_input);// Note that the user input has the newline character 
	}


	return 0;
}
