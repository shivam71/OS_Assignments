#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Job{
    char *PID;
    float T_gen;
    float T_comp;
};

int job_comparator(const void *v1, const void *v2)
{
    const struct Job *p1 = (struct Job *)v1;
    const struct Job *p2 = (struct Job *)v2;
    if(p1->T_gen != p2->T_gen)
        return (p1->T_gen > p2->T_gen);
    return (p1->T_comp > p2->T_comp);
}

void print_jobs(struct Job arr[], int n){
    int i;
    for (i = 0; i < n; ++i){
        printf("%s %f %f\n", arr[i].PID, arr[i].T_gen, arr[i].T_comp);
    }
}

int main(int argc, char* argv[]){
    struct Job jobs_list[200];
    
    //if input has correct number of arguments
    if(argc != 2){
        printf("Invalid Input\n");
    }
    
    //reading from input .txt file
    FILE* fp = fopen(argv[1], "r");
    if(NULL == fp){
        printf("File can't be opened\n");
        return 1;
    }
    // reading line by line, max 256 bytes
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];
    int num_jobs = 0;
    while (fgets(buffer, MAX_LENGTH, fp)){
        //tokenise buffer
        char *word = strtok(strdup(buffer)," ");
        jobs_list[num_jobs].PID = strdup(word);
        word = strtok(NULL," ");
        jobs_list[num_jobs].T_gen = atoi(strdup(word));
        word = strtok(NULL,"\n");
        jobs_list[num_jobs].T_comp = atoi(strdup(word));
        num_jobs += 1;
    }
    fclose(fp);    
    
    //sorting the jobs
    print_jobs(jobs_list, num_jobs);
    qsort((void*)jobs_list, num_jobs, sizeof(jobs_list[0]), job_comparator);
    print_jobs(jobs_list, num_jobs);
    
    return 0;
}

