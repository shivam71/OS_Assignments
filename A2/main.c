#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Job{
    char *PID;
    double T_gen;
    double T_comp;
};

struct Process{
    char* PID;
    double T_gen;
    double T_comp;
    double T_first_sch;// time when first scheduled
    double T_finish;
    double RT;// Response time
    double TAT;// Turnaround time
};

struct CPU_burst{
    char* PID;
    double T_start;
    double T_end;
};

int max(int num1, int num2){
    return (num1 > num2 ) ? num1 : num2;
}

int job_comparator(const void *v1, const void *v2){
    const struct Job *p1 = (struct Job *)v1;
    const struct Job *p2 = (struct Job *)v2;
    if(p1->T_gen != p2->T_gen)
        return (p1->T_gen > p2->T_gen);
    return (p1->T_comp > p2->T_comp);
}

void print_process_exec_seq(struct CPU_burst* CPU_burst_ls,int num_bursts){
    for(int idx=0;idx<num_bursts;idx++){
        printf("%s %g %g ",CPU_burst_ls[idx].PID,CPU_burst_ls[idx].T_start,CPU_burst_ls[idx].T_end);
    }
    printf("\n");
}

void compute_print_metrics(struct Process* p_list, int num_p){
    // compute the avg RT and avg TAT
    double avg_RT,avg_TAT;
    avg_RT = 0.0;
    avg_TAT = 0.0;
    double num_pr = (double) num_p;
    for(int idx=0;idx<num_p;idx++){
        avg_RT+=p_list[idx].RT;
        avg_TAT+=p_list[idx].TAT;
    }
    avg_RT/=num_pr;
    avg_TAT/=num_pr;
    printf("%g %g\n",avg_TAT,avg_RT);
}


void run_FCFS(struct Job* wl, int num_jobs){
    //num jobs == num proccesses = num bursts, in FCFS
    struct Process p_list[num_jobs];
    struct CPU_burst CPU_burst_ls[num_jobs]; //make this number a parameter
    
    double curr_time = 0.0;
    for(int i = 0; i < num_jobs; i++)
    {
        p_list[i].PID = wl[i].PID;
        p_list[i].T_gen = wl[i].T_gen;
        p_list[i].T_comp = wl[i].T_comp;
        p_list[i].T_first_sch = max(curr_time, wl[i].T_gen);
        p_list[i].T_finish = p_list[i].T_first_sch + p_list[i].T_comp;
        p_list[i].RT = p_list[i].T_first_sch - p_list[i].T_gen;
        p_list[i].TAT = p_list[i].T_finish - p_list[i].T_gen;
        
        CPU_burst_ls[i].PID = wl[i].PID;
        CPU_burst_ls[i].T_start = p_list[i].T_first_sch;
        CPU_burst_ls[i].T_end = p_list[i].T_finish;
        
        curr_time = p_list[i].T_finish;
    }
    printf("FCFS : ");
    print_process_exec_seq(CPU_burst_ls, num_jobs);
    compute_print_metrics(p_list, num_jobs);
    return;
}

void run_SJF(struct Job* wl, int num_jobs){
    //num jobs == num proccesses = num bursts, in FCFS
    struct Process p_list[num_jobs];
    struct CPU_burst CPU_burst_ls[num_jobs];
    
    double curr_time = 0.0;
    for(int i = 0; i < num_jobs; i++)
    {
        p_list[i].PID = wl[i].PID;
        p_list[i].T_gen = wl[i].T_gen;
        p_list[i].T_comp = wl[i].T_comp;
        p_list[i].T_first_sch = max(curr_time, wl[i].T_gen);
        p_list[i].T_finish = p_list[i].T_first_sch + p_list[i].T_comp;
        p_list[i].RT = p_list[i].T_first_sch - p_list[i].T_gen;
        p_list[i].TAT = p_list[i].T_finish - p_list[i].T_gen;
        
        CPU_burst_ls[i].PID = wl[i].PID;
        CPU_burst_ls[i].T_start = p_list[i].T_first_sch;
        CPU_burst_ls[i].T_end = p_list[i].T_finish;
        
        curr_time = p_list[i].T_finish;
    }
    printf("SJF : ");
    print_process_exec_seq(CPU_burst_ls, num_jobs);
    compute_print_metrics(p_list, num_jobs);
    return;
}

void run_experiments(struct Job* wl, int num_jobs){
    run_FCFS(wl, num_jobs);
    run_SJF(wl, num_jobs);
}

void print_jobs(struct Job arr[], int n){
    int i;
    for (i = 0; i < n; ++i){
        printf("%s %f %f\n", arr[i].PID, arr[i].T_gen, arr[i].T_comp);
    }
}

int main(int argc, char* argv[]){
    struct Job jobs_list[200];
    struct Process process_list[200];
    
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
        char* ptr;
        char *word = strtok(strdup(buffer)," ");
        jobs_list[num_jobs].PID = strdup(word);
        word = strtok(NULL," ");
        jobs_list[num_jobs].T_gen = strtod(strdup(word), &ptr);
        word = strtok(NULL,"\n");
        jobs_list[num_jobs].T_comp = strtod(strdup(word), &ptr);
        num_jobs += 1;
    }
    fclose(fp);
    
    //sorting the jobs
    //print_jobs(jobs_list, num_jobs);
    qsort((void*)jobs_list, num_jobs, sizeof(jobs_list[0]), job_comparator);
    print_jobs(jobs_list, num_jobs);
    
    run_experiments(jobs_list, num_jobs);
    return 0;
}

