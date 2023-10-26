#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>
// Custom datatypes

typedef struct rw_file_lock{
	sem_t rlock;
	sem_t wlock;
	sem_t reading_poss;
	sem_t writing_poss;
	int reader_count;
	int writer_count;
} rw_file_lk;

typedef struct _file_obj{
	char* file_name;
	rw_file_lk* file_lock;

} file_obj;

// Global variables
 
int readers_with_lk;
int writers_with_lk;
int files_arr_size;
int files_arr_cap;
int threads_arr_cap;
int threads_arr_size;
char* user_input;
file_obj* files_arr;
char** parsed_input;
pthread_t* threads_arr;
pthread_mutex_t total_rw_info_lock;
// Helper functions 

void init_gl_vars(){
	parsed_input = malloc(10*sizeof(char*));
	readers_with_lk = 0;
	writers_with_lk = 0;
	files_arr_size = 0;
	files_arr_cap = 40;// make it dynamic
	threads_arr_size = 0;
	threads_arr_cap = 100;
	threads_arr = malloc(100*sizeof(pthread_t));
	files_arr = malloc(40*sizeof(file_obj));
}

void expand_threads_arr(){
	threads_arr = realloc(threads_arr,2*threads_arr_cap*sizeof(pthread_t));
	threads_arr_cap = pthreads_arr_cap*2;
}

int get_total_readers(){
	int count = 0;
	pthread_mutex_lock(&total_rw_info_lock);
	count = readers_with_lk;
	pthread_mutex_unlock(&total_rw_info_lock);
	return count;
}

void allocate_mem_input(){
	user_input = malloc(2048*sizeof(char));
}

void readlock(rw_file_lk* rw_lock){


}
void readunlock(rw_file_lk* rw_lock){


}
void writelock(rw_file_lk* rw_lock){


}
void writeunlock(rw_file_lk* rw_lock){


}


void read(char*fname,rw_file_lk* flock){


}


void write1(char* fname_1,char* fname_2,rw_file_lk* flock_1,rw_file_lk* flock_2){


}

void write2(char* fname_1,rw_file_lk* flock_1,char* line){


}


void init_lock(rw_file_lk* lock_ptr){
	lock_ptr->reader_count = 0;
	lock_ptr->writer_count = 0;
	sem_init(&(lock_ptr->rlock),0,1);
	sem_init(&(lock_ptr->wlock),0,1);
	sem_init(&(lock_ptr->reading_poss),0,1);
	sem_init(&(lock_ptr->writing_poss),0,1);
}

rw_file_lk* get_file_lock_ptr(char* fname){
// Traverse the array to get the file_obj if exists 
// Initialize the last element of the array for the current file 
// return the lock 
	bool found = false;
	rw_file_lk* lock_ptr;
	char* name;
	for(int pos=0;pos<files_arr_size;pos++){
		name = files_arr[pos].file_name;
		if(strcmp(name,fname)==0){
			found = true;
			lock_ptr = files_arr[pos].file_lock;
			break;
		}
	}

	if(!found){
		files_arr[files_arr_size].file_name = strdup(fname);
		files_arr[files_arr_size].file_lock = malloc(sizeof(rw_file_lk));
		init_lock(files_arr[files_arr_size]);
		lock_ptr = files_arr[files_arr_size];
		files_arr_size++;// assuming we don't need to change the capacity 
	}
	return lock_ptr;

}

void parse_input(){
	int num_tokens = 1;
	int token_len;
	const char* delims = "-";
	char* user_input_copy = strdup(user_input);
	char* token = strtok(user_input_copy," ");
	while(token!=NULL){
		parsed_input[num_tokens-1]= strdup(token);
		num_tokens++;
		if(num_tokens==4){
			token = strtok(NULL,"\n");
		}else{
			token = strtok(NULL," ");
		}
	
	}
	while(num_tokens<4){
		num_tokens++;
		parsed_input[num_tokens-1] = NULL;
	}
	for(int i = 0;i<4;i++){
                printf("%s\n",parsed_input[i]);
        }
}


void wait_all_finish(){
	for(int i = 0;i<threads_arr_size;i++){
		pthread_join(&threads_arr[i],NULL);
	}
}


int main(){
    init_gl_vars(); 
    char* command;
    char* fname_1;
    char* fname_2;
    char* line;
    char* type;
    rw_file_lk* f1_lk;
    rw_file_lk* f2_lk;
    allocate_mem_input();
    while(gets(user_input)){
	user_input[strcspn(user_input,"\n")] = '\0';
	if(strlen(user_input)==0) continue;
	parse_input();
	command  = parsed_input[0];
	if(threads_arr_size==threads_arr_cap) expand_threads_arr();
	threads_arr_size++;
	if(strcmp(command,"read")==0){
		fname_1 = parsed_input[1];
		f1_lk = get_file_lock(fname_1);
		pthread_create(threads_arr[threads_arr_size-1],NULL,read,args);
	}else if(strcmp(command,"write")==0){
		type = parsed_input[1];
		fname_1 = parsed_input[2];
		f1_lk = get_file_lock(fname_1);
		if(strcmp(type,"1")==0){
			fname_2 = parsed_input[3];
			f2_lk = get_file_lock(fname_2);
		}else{
			line = parsed_input[3];
		}	
	}else if(strcmp(command,"exit")==0){
		// wait till all the child threads have completed 
		wait_all_finish();
	}else{
		printf("Invalid input try again!!\n");
		continue;
	}
	allocate_mem_input();
    }
}
