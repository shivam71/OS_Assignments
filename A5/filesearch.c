#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<dirent.h>
#include<sys/stat.h>

char* target;
char path[256];// For storing the path 
int type; // 0 file 1 dir and 2 link 
bool found;
int find_type(mode_t entry_mode){ // using the file stats determines the type
	if(S_ISDIR(entry_mode)){//Check directory 
		return 1;
        }else if(S_ISREG(entry_mode)){//Check Regular file
		return 0;
        }else if(S_ISLNK(entry_mode)){//Check if link 
		return 2;
        }else{
		return -1;// Any other
	}

}

void print_path(int type){
	if(type==0){
           printf("File found: %s/%s\n",path,target);
        }else if(type==1){
           printf("Directory found: %s/%s\n",path,target);
        }else{
           printf("Link found: %s/%s\n",path,target);
       }
}

void search_directory(){
	// Recursively searches for the target in the current directory 
	DIR* cdir_ptr = opendir(".");
	struct dirent* entry;
	struct stat entry_stats;
	mode_t entry_mode;
	int entry_type;
	while(entry = readdir(cdir_ptr)){
		// skip the current dir and the parent dir entries 
		if(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0){
			continue;
		}
		lstat(entry->d_name,&entry_stats);// get the stats of the target 
		entry_mode = entry_stats.st_mode;
		entry_type = find_type(entry_mode);
		if(entry_type==-1) continue;
		if(strcmp(entry->d_name,target)==0){// if the name of the entry matches target then store the path and return 
			getcwd(path,256);
			type = entry_type;
			found=true;
			print_path(type);
		}
		// recursively search the sub directory in case the entry is a directory 
		if(S_ISDIR(entry_mode)){
			chdir(entry->d_name);// Enter that directory 
			search_directory();
			chdir("..");// return back to the cdir
		}
	}
}



int main(int argc,char *argv[]){
	if(argc!=2){
		printf("Error wrong input just pass the target name no other string \n");
		return 1;
	}
	target = argv[1];// Traget name
	found = false;
	search_directory();
	if(found){
		// already the path was printed 
	}else{
		printf("The target '%s' was not found in the current directory\n",target);
	}
	return 0;
}
