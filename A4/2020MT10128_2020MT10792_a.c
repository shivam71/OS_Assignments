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


typedef struct _thread_args{
	char* fname_1;
	char* fname_2;
	char* line;
	rw_file_lk* flock_1;
	rw_file_lk* flock_2;
} thread_args;

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
	threads_arr_cap = threads_arr_cap*2;
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
	sem_wait(&(rw_lock->reading_poss));
	sem_wait(&(rw_lock->rlock));
		if(rw_lock->reader_count==0){
			sem_wait(&(rw_lock->writing_poss));// if the first reader has to wait then subsequent readers cannot acquire the rlock
		}
	rw_lock->reader_count++;
	sem_post(&(rw_lock->rlock));
	pthread_mutex_lock(&total_rw_info_lock);
	readers_with_lk++;
	pthread_mutex_unlock(&total_rw_info_lock);
	sem_post(&(rw_lock->reading_poss));
}

void readunlock(rw_file_lk* rw_lock){
	sem_wait(&(rw_lock->rlock));
	rw_lock->reader_count--;
	if(rw_lock->reader_count==0) sem_post(&(rw_lock->writing_poss));
	pthread_mutex_lock(&total_rw_info_lock);
        readers_with_lk--;
        pthread_mutex_unlock(&total_rw_info_lock);
	sem_post(&(rw_lock->rlock));
}

void writelock(rw_file_lk* rw_lock){
	sem_wait(&(rw_lock->wlock));
	if(rw_lock->writer_count==0) sem_wait(&(rw_lock->reading_poss));
	rw_lock->writer_count++;// doesn't actually have the lock	
	sem_post(&(rw_lock->wlock));
	sem_wait(&(rw_lock->writing_poss));
	pthread_mutex_lock(&total_rw_info_lock);
        writers_with_lk++;
        pthread_mutex_unlock(&total_rw_info_lock);

}
void writeunlock(rw_file_lk* rw_lock){
	sem_wait(&(rw_lock->wlock));
        rw_lock->writer_count--;
        if(rw_lock->writer_count==0){
		sem_post(&(rw_lock->reading_poss));
	}
	sem_post(&(rw_lock->writing_poss));
        pthread_mutex_lock(&total_rw_info_lock);
        writers_with_lk--;
        pthread_mutex_unlock(&total_rw_info_lock);
        sem_post(&(rw_lock->wlock));
}


int read_file(char** string_read,FILE* file_ptr){
	int bytes_read = 0;
	int cap = 100;
	//printf("Reading a file\n");
	string_read[0] = malloc(cap*sizeof(char));
	int c = fgetc(file_ptr);
	while(c!=EOF){
		bytes_read++;
		if(bytes_read==cap){
			cap*=2;
			string_read[0] = realloc(string_read[0],cap*sizeof(char));
		}
		string_read[0][bytes_read-1] = (char)c;
		c = fgetc(file_ptr);
	}
	string_read[0][bytes_read] = '\0';
	//printf("Reading complete\n");
	return bytes_read;
}

char* read_file1(FILE* file_ptr){
        int bytes_read = 0;
        int cap = 10000;
        char* string_read = malloc(cap*sizeof(char));
        int c = fgetc(file_ptr);
        while(c!=EOF){
                bytes_read++;
                if(bytes_read==cap){
                        cap=2*cap;
                        string_read = realloc(string_read,cap*sizeof(char));
                }
                string_read[bytes_read-1] = (char)c;
                c = fgetc(file_ptr);
        }
        string_read[bytes_read] = '\0';
        printf("Reading complete\n");
        return string_read;
}



int write_file(char* string_write,FILE* file_ptr){
	int bytes_written = 0;// include the one byte corresponding to '\n'
	// add a new line
	long f_ptr_pos = ftell(file_ptr);
	printf("%ld file pointer pos",f_ptr_pos);
	if(f_ptr_pos!=((long)0)){
		printf("Printing new line");
		bytes_written++;
	fputc('\n',file_ptr);// What if the file is empty ?
	}
	char c = *string_write;
	while(c!='\0'){
		bytes_written++;
		fputc(c,file_ptr);
		string_write++;
		c = *string_write;
	}
	return bytes_written;
}
void* read1(thread_args* args){
	//printf("Here \n");
	int bytes_read = 0;
	char** string_read = malloc(sizeof(char*));
	readlock(args->flock_1);
	//printf("Here 1\n");
	FILE* file_ptr = fopen(args->fname_1,"r");
	bytes_read = read_file(string_read,file_ptr);
	fclose(file_ptr);
	// Why not acquire a lock here to print the number of readers and writers
	//printf("About to acquire lock\n");
	pthread_mutex_lock(&total_rw_info_lock);
       	printf("read %d bytes with %d reader and %d writer present\n",bytes_read,readers_with_lk,writers_with_lk);
        pthread_mutex_unlock(&total_rw_info_lock);
	readunlock(args->flock_1);
	return NULL;
}


void* write1(thread_args* args){
	//char* string_read= NULL;
	printf("Before seg 0\n");
	int bytes_read = 0;
	char* string_read = NULL;
	int bytes_written = 0;
	readlock(args->flock_2);
	FILE* file_ptr = fopen(args->fname_2,"r");
        string_read = read_file1(file_ptr);
	printf("Before seg 2\n");
	fclose(file_ptr);
	readunlock(args->flock_2); 
	printf("Before seg\n");


	writelock(args->flock_1);
	file_ptr = fopen(args->fname_1,"a");// check what it returns
	bytes_written = write_file(string_read,file_ptr);
	fclose(file_ptr);
	pthread_mutex_lock(&total_rw_info_lock);
        printf("wrote %d bytes with %d reader and %d writer present\n",bytes_written,readers_with_lk,writers_with_lk);
        pthread_mutex_unlock(&total_rw_info_lock);
	writeunlock(args->flock_1);
	return NULL;
}

void* write2(thread_args* args){
	int bytes_written = 0;
	writelock(args->flock_1);
	FILE* file_ptr = fopen(args->fname_1,"a");
	bytes_written = write_file(args->line,file_ptr);
	fclose(file_ptr);
	pthread_mutex_lock(&total_rw_info_lock);
        printf("wrote %d bytes with %d reader and %d writer present\n",bytes_written,readers_with_lk,writers_with_lk);
        pthread_mutex_unlock(&total_rw_info_lock);
	writeunlock(args->flock_1);
	return NULL;
}


void init_lock(rw_file_lk* lock_ptr){
	lock_ptr->reader_count = 0;
	lock_ptr->writer_count = 0;
	sem_init(&(lock_ptr->rlock),0,1);
	sem_init(&(lock_ptr->wlock),0,1);
	sem_init(&(lock_ptr->reading_poss),0,1);
	sem_init(&(lock_ptr->writing_poss),0,1);
}

void remove_trailing_spaces(char* str_org){
	if(str_org==NULL) return;
	int pos = strlen(str_org);
	pos--;	
	char c  = str_org[pos];	
	while(c==' '){
		pos--;
		c = str_org[pos];
	}
	str_org[pos+1] = '\0'; 
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
		init_lock(files_arr[files_arr_size].file_lock);
		lock_ptr = files_arr[files_arr_size].file_lock;
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
			// Still a problem with trailing spaces
			remove_trailing_spaces(token);
		}else{
			token = strtok(NULL," ");
		}
	
	}
	/*while(num_tokens<=4){
		parsed_input[num_tokens-1] = NULL;
		num_tokens++;
	}
	for(int i = 0;i<4;i++){
                printf("%s\n",parsed_input[i]);
        }*/
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
    thread_args* args;
    sleep(10);
    while(gets(user_input)){
	user_input[strcspn(user_input,"\n")] = '\0';
	if(strlen(user_input)==0) continue;
	parse_input();
	command  = parsed_input[0];
	if(threads_arr_size==threads_arr_cap) expand_threads_arr();
	threads_arr_size++;
	args = malloc(sizeof(thread_args));
	if(strcmp(command,"read")==0){
	//	printf("Here1 \n");
		fname_1 = parsed_input[1];
	//	printf("Here2 \n");
		f1_lk = get_file_lock_ptr(fname_1);
		args->fname_1 = fname_1;
		args->flock_1 = f1_lk;
		pthread_create(&threads_arr[threads_arr_size-1],NULL,read1,args);
	}else if(strcmp(command,"write")==0){
		type = parsed_input[1];
		fname_1 = parsed_input[2];
		f1_lk = get_file_lock_ptr(fname_1);
		args->fname_1 = fname_1;
                args->flock_1 = f1_lk;
		if(strcmp(type,"1")==0){
			fname_2 = strdup(parsed_input[3]);
			//printf("%sNOTRAIL",fname_2);
			f2_lk = get_file_lock_ptr(fname_2);
			args->fname_2 = fname_2;
                	args->flock_2 = f2_lk;
			pthread_create(&threads_arr[threads_arr_size-1],NULL,write1,args);
		}else{
			line = parsed_input[3];
			args->line = line;
			pthread_create(&threads_arr[threads_arr_size-1],NULL,write2,args);
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
