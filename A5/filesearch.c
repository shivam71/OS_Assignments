#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdbool.h>
#include<dirent.h>
#include<sys/stat.h>

char* target;
char path[256];
int type; // 0 file 1 dir and 2 link 

int find_type(mode_t entry_mode){
	if(S_ISDIR(entry_mode)){
		return 1;
        }else if(S_ISREG(entry_mode)){
		return 0;
        }else if(S_ISLNK(entry_mode)){
		return 2;
        }else{
		return -1;
	}
}

bool search_directory(){
	DIR* cdir_ptr = opendir(".");
	struct dirent* entry;
	struct stat entry_stats;
	mode_t entry_mode;
	bool found;
	int entry_type;
	while(entry = readdir(cdir_ptr)){
		// skip the current dir and the parent dir entries 
		if(strcmp(".",entry->d_name)==0 || strcmp("..",entry->d_name)==0){
			continue;
		}
		lstat(entry->d_name,&entry_stats);
		entry_mode = entry_stats.st_mode;
		entry_type = find_type(entry_mode);
		if(entry_type==-1) continue;
		if(strcmp(entry->d_name,target)==0){
			getcwd(path,256);
			type = entry_type;
			return true;
		}
		// recursively search the sub directory 
		if(S_ISDIR(entry_mode)){
			chdir(entry->d_name);
			found = search_directory();
			chdir("..");// return back to the cdir
			if(found) return true;
		}
	}
	return false;
}



int main(int argc,char *argv[]){
	if(argc!=2){
		printf("Error\n");
		return 1;
	}
	target = argv[1];
	if(search_directory()){
		if(type==0){
			printf("File found: %s/%s\n",path,target);
		}else if(type==1){
			printf("Directory found: %s/%s\n",path,target);
		}else{
			printf("Link found: %s/%s\n",path,target);
		}
	}else{
		printf("The target %s was not found in the current directory\n",target);
	}
	return 0;
}
