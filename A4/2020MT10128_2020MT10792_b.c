#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>


// Custom data types

typedef _node{
	int val;
	int height;
	t_node* left;
	t_node* right;
	pthread_mutex_t* node_lk;
} t_node;



// Global variables
 
char* user_input;
char** parsed_input;
t_node* root_ptr;
// Helper functions 

void init_gl_vars(){
	parsed_input = malloc(2*sizeof(char*));
}

void allocate_mem_input(){
	user_input = malloc(2048*sizeof(char));
}








void parse_input(){
	int num_tokens = 0;
	char* user_input_copy = strdup(user_input);
	char* token = strtok(user_input_copy," ");
	while(token!=NULL){
		num_tokens++;
		parsed_input[num_tokens-1]= strdup(token);
		token = strtok(NULL," ");
	}
	while(num_tokens<2){
		num_tokens++;
		parsed_input[num_tokens-1] = NULL;
	}
	/*for(int i = 0;i<2;i++){
                printf("%s\n",parsed_input[i]);
        }*/
}



int main(){
    init_gl_vars();
    allocate_mem_input();
    while(gets(user_input)){
	user_input[strcspn(user_input,"\n")] = '\0';
	parse_input();
	printf("%s\n",parsed_input[0]);
	allocate_mem_input();
    }
}
