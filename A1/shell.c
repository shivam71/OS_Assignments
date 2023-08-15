#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>

#define clear() printf("\e[1;1H\e[2J")

// Create a global history
char unix_commands[]= "ls-cat-echo-sleep-grep-wc";
char other_commands[]="cd-history";
int initial_capacity=1;
char** history;//(char**)malloc(initial_capacity*sizeof(char*));
int history_capacity=1;
int history_size=0;

void init_shell()
{
    clear();
    printf("\n\n\n\n******************"
        "************************");
    printf("\n\n\n\t\tMTL458");
    printf("\n\n\t\tAssignment 1");
    printf("\n\n\n\n*******************"
        "***********************");
    printf("\n");
    sleep(1);
    clear();
}

void history_resize(){
    char **temp = history;
    history = (char**)realloc(history,history_capacity*sizeof(char*));
    if(history==NULL){
        history = temp;
        printf("Error history cannot be updated\n");
    }else{
        
    }
}

void print_history(){
    int c_index = 0;
    while(c_index<history_size){
        printf("%d. %s\n",c_index+1,history[c_index]);
        c_index++;
    }
    return;

}

void update_history(char* command){
    
    if(history_size<history_capacity){
        
    }else{
        history_capacity=history_capacity*2;
        history_resize();
    }

    history[history_size]=strdup(command);
    history_size++;

}


// How to do the history
int valid(char *command,char *valid_commands){
    char *v_commands = strdup(valid_commands);// going to use strtok which modifies the strinf so creating a copy
    char *v_command = strtok(v_commands,"-");
    int valid = 0;
    while(v_command!=NULL){
        if(strcmp(v_command,command)==0){
            valid = 1;
        }
        v_command = strtok(NULL,"-");
    }
    return valid;
}

void piped_execute(char **pipe1, char **pipe2){
    // when doing piping the execute might return an error ->check
    
    //also assumed pipe commands will only have inherenet unix commands
    int fds[2];
    pipe(fds);
    
    int f =0;
    if(valid(pipe1[0],unix_commands) && valid(pipe2[0],unix_commands)){
        f=1;
    }
    if(!f){
        printf("Invalid input\n");
        return;
    }
    
    //printf("here\n");
    if(fork()==0){
        dup2(fds[0], STDIN_FILENO);
        close(fds[1]);
        close(fds[0]);
        execvp(pipe2[0],pipe2);
        //printf("here\n");
    }
    if(fork() == 0){
        dup2(fds[1], STDOUT_FILENO);
        close(fds[0]);
        close(fds[1]);
        execvp(pipe1[0],pipe1);
        printf("here\n");
    }    
    close(fds[1]);
    close(fds[0]);
    wait(NULL);
}

void execute(char *args[],char* redir){
    // when doing piping the execute might return an error ->check
     
    int f =0;
    if(valid(args[0],unix_commands)){
        f=1;
    }
    if(f){
           int pid = fork();
                if(pid<0){
                    printf("Invalid input\n");
                }else if(pid==0){
            //printf("reached here\n");
            //printf("%s\n",redir);
            if(strcmp(redir,"\0")!=0){//assume valid file name check this before
                close(STDOUT_FILENO);
                    int status = open(redir,O_CREAT|O_WRONLY|O_TRUNC,S_IRWXU);// can throw error ? handle
                   if(status==-1) printf("Error\n");
            }
                    execvp(args[0],args);
                }else{
                    int wc = wait(NULL);
                }
    }else{
        if(strcmp(args[0],"cd")==0){
            int num_args = 2;// how to calculate
            if(args[num_args]!=NULL){
                printf("Invalid input\n");
            }else{
                int status = chdir(args[1]);
                if(status==-1){
                    printf("Directory doesn't exists\n");
                }
            }

        }else if(strcmp(args[0],"history")==0){
            int num_args=1;
            if(args[num_args]!=NULL){
                printf("Invalid input\n");
            }else{
                print_history();
            }
        }

    }

}


int piped_parse(char *u_input,char *piped_input[]){
    char *user_input = strdup(u_input);
    
    int i = 0;
    char *word = strtok(strdup(user_input),"|");
    while(word!=NULL){
        piped_input[i]=strdup(word);
        word = strtok(NULL,"|");
        i+=1;
    }
    piped_input[i]=NULL;
    //printf("%d\n", i);
    if(i == 2)//just taking two piped commands right now
    {
        return 1;
    }
    else
    {
        printf("Invalid Input\n");
        return 0;
    }
}

int parse_simple(char *u_input,char *args[],char* redir){
    char *user_input = strdup(u_input);
    if(strcspn(user_input,">")!=strlen(user_input)){
                                // with redirection
        char *word = strtok(user_input,">");// modifies user_input string
        char *file_n = strdup(strtok(NULL,">"));// duplicate a null string ?
        if(strtok(NULL,">")!=NULL || file_n==NULL){
            printf("Error invalid input\n");
            while(strtok(NULL,">")!=NULL){}
            return 0;
        }
        while(strtok(NULL,">")!=NULL){}
        //printf("%s|\n", word);
        
        char *word1 = strtok(word," ");
        while(strtok(NULL," ")!=NULL){}
        //printf("%s|\n", word1);
        
        char *file_n1 = strtok(file_n," ");
        //printf("%s|\n", file_n1);
        while(strlen(file_n1) < 4)
        {
            file_n1 = strtok(NULL, " ");
        }
        while(strtok(NULL," ")!=NULL){}
        //printf("%s|\n", file_n1);


        strcpy(redir,file_n1);

        }else{
                strcpy(redir,"\0");               // no redirection
        }
    char *word = strtok(strdup(user_input)," ");// multiple spaces as problem ?
    // alternative to strtok find
    char *command  = strdup(word);
    while(word!=NULL){
        word=strtok(NULL," ");
    }
    if(valid(command,unix_commands) || valid(command,other_commands) ){
        int i = 0;
        char *word = strtok(strdup(user_input)," ");
        while(word!=NULL){
            args[i]=strdup(word);
            word = strtok(NULL," ");
            i+=1;
        }
        args[i]=NULL;// marks the end

        
        return 1;
    }else{
        printf("Invalid instruction try again\n");
        return 0;
    }
}

int main(){
    // Store the previously executed commands
    init_shell();
    history_resize();
    while(1){
        char *args[2048];// problem a bit
        char *piped_input[2048];
        char *pipe1[2048];
        char *pipe2[2048];
        char redir[2048];// fixed ?
        int f=0;
        char user_input[2048];// right now fixed
        printf("MTL458>");
        fflush(stdout);
        fgets(user_input,sizeof(user_input),stdin);
        // Handle empty string
        user_input[strcspn(user_input,"\n")]='\0';
        if(strchr(user_input,'|')!=NULL){
            // piped instruction
            //printf("Sorry piped instrcutions are not supported right now\n");
            int valid = piped_parse(user_input, piped_input);
            if(valid){
                int valid1 = parse_simple(piped_input[0], pipe1, redir);
                int valid2 = parse_simple(piped_input[1], pipe2, redir);
                //printf("%s|\n", pipe1[0]);
                if(valid1 && valid2){
                    //printf("here\n");
                    piped_execute(pipe1, pipe2);
                }
            }else{
                continue;
            }
        }else{
            // non piped instruction
            int valid = parse_simple(user_input,args,redir);// for invalid input the parse function prints an error
                   if(valid){
                
                execute(args,redir);

            }
        }
        update_history(user_input);
        // update the stored commands
        // String length problem if input is less than hundered characters ?
        // Note that the user input has the newline character
    }


    return 0;
}
