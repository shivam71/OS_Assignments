#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>


int main(){
	while(1){
		char user_input[100];
		printf("MTL458>");
		fflush(stdout);
		fgets(user_input,sizeof(user_input),stdin);// String length problem if input is less than hundered characters ?
		printf("User input was %s",user_input);// Note that the user input has the newline character 
	}


	return 0;
}
