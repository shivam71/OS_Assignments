#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

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

struct Process_Node{// use this implementing a     queue using linked list  use for RR and MLFQ
    struct Process_Node* next;// pointer to the next Node in the list
    struct Process* process;// pointer to the process associated with this node (could have this for Job <-> Process )
    double T_comp_left;//  Be careful while comparing double == 0  use double < epsilion
};

struct Process_Queue{
    struct Process_Node* head;// deletion update this // create a function
    struct Process_Node* tail;// insertion update this // create a function
    int size;
};

double max_double(double first,double second){
    return (first>second) ? first : second;
}

double min_double(double first,double second){
    return (first<second) ? first : second;
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


void run_FCFS(struct Job* wl,int num_jobs){
    double curr_time = 0.0;
    struct Process* p_list = malloc(sizeof(struct Process)*1);
    p_list = (struct Process*) realloc(p_list,sizeof(struct Process)*num_jobs);
    struct CPU_burst* CPU_burst_ls = malloc(sizeof(struct CPU_burst)*1);
    CPU_burst_ls =(struct CPU_burst*)realloc(CPU_burst_ls,sizeof(struct CPU_burst)*num_jobs);// cannot do in all scheduling algos before hand
        for(int i = 0; i < num_jobs; i++)
        {
            p_list[i].PID = wl[i].PID;
            p_list[i].T_gen = wl[i].T_gen;
            p_list[i].T_comp = wl[i].T_comp;
            p_list[i].T_first_sch = max_double(curr_time, wl[i].T_gen);
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
    double curr_time = 0.0;
    struct Process* p_list = malloc(sizeof(struct Process)*1);
    p_list = (struct Process*) realloc(p_list,sizeof(struct Process)*num_jobs);
    struct CPU_burst* CPU_burst_ls = malloc(sizeof(struct CPU_burst)*1);
    CPU_burst_ls =(struct CPU_burst*)realloc(CPU_burst_ls,sizeof(struct CPU_burst)*num_jobs);// cannot do in all scheduling algos before hand
        for(int i = 0; i < num_jobs; i++)
        {
            p_list[i].PID = wl[i].PID;
            p_list[i].T_gen = wl[i].T_gen;
            p_list[i].T_comp = wl[i].T_comp;
            p_list[i].T_first_sch = max_double(curr_time, wl[i].T_gen);
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

void run_RR(struct Job* wl, int num_jobs, double TsRR){
    double *arr;
    arr = (double *)malloc(num_jobs * sizeof(int));
    for(int i = 0; i < num_jobs; i++){arr[i] = 99999999.9;}
    
    struct Job* w2 = malloc(sizeof(struct Job)*1);
    w2 = (struct Job*) realloc(w2,sizeof(struct Job)*num_jobs);
    memcpy(w2, wl, num_jobs * sizeof(*w2));
    
    struct Process* p_list = malloc(sizeof(struct Process)*1);
    p_list = (struct Process*) realloc(p_list,sizeof(struct Process)*num_jobs);
    struct CPU_burst* CPU_burst_ls = malloc(sizeof(struct CPU_burst)*1);
    CPU_burst_ls =(struct CPU_burst*)realloc(CPU_burst_ls,sizeof(struct CPU_burst)*num_jobs * 10);// cannot do in all scheduling algos before hand
    int j = 0; //cpu_burst_count
    double curr_time = 0.0;
    int i = 0;
    int jobs_left = num_jobs;
    bool context_switch = false;
    int last_process = -1;
    while(jobs_left != 0){

        if(CPU_burst_ls[j - 1].PID == w2[i].PID){
            context_switch = false;
        }
        else{
            context_switch = true;
            last_process = i;
        }
        
        if(w2[i].T_comp <= TsRR && w2[i].T_comp > 0){
            double curr_time_old = curr_time;
            curr_time += w2[i].T_comp;
            w2[i].T_comp = 0;
            if(!context_switch){
                j--;
                CPU_burst_ls[j].T_end = curr_time;
            }
            else{
                CPU_burst_ls[j].PID = w2[i].PID;
                CPU_burst_ls[j].T_start = curr_time_old;
                CPU_burst_ls[j].T_end = curr_time;
            }
            
            p_list[i].PID = w2[i].PID;
            p_list[i].T_gen = w2[i].T_gen;
            arr[i] = min_double(curr_time_old, arr[i]);
            p_list[i].T_first_sch = arr[i];
            p_list[i].T_finish = curr_time;
            p_list[i].T_comp = p_list[i].T_finish - p_list[i].T_first_sch;
            p_list[i].RT = p_list[i].T_first_sch - p_list[i].T_gen;
            p_list[i].TAT = p_list[i].T_finish - p_list[i].T_gen;
            
            jobs_left--; j++;
        }
        else if(w2[i].T_comp > 0){
            if(!context_switch){
                j--;
                CPU_burst_ls[j].T_end = curr_time + TsRR;
            }
            else{
                CPU_burst_ls[j].PID = w2[i].PID;
                CPU_burst_ls[j].T_start = curr_time;
                CPU_burst_ls[j].T_end = curr_time + TsRR;
            }
            arr[i] = min_double(curr_time, arr[i]);
            p_list[i].T_first_sch = arr[i];
            w2[i].T_comp -= TsRR;
            curr_time += TsRR;
            j++;
        }
        if(i == num_jobs - 1){
            i = 0;
        }
        else if(w2[i + 1].T_gen <= curr_time){
            i++;
        }
        else{
            i = 0;
        }
    }
    printf("RR : ");
    print_process_exec_seq(CPU_burst_ls, j);
    compute_print_metrics(p_list, num_jobs);
    return;
}

void run_experiments(struct Job* wl, int num_jobs, double TsRR){
    run_FCFS(wl, num_jobs);
    run_RR(wl, num_jobs, TsRR);
    run_SJF(wl, num_jobs);
    //run_SRTF(w1, num_jobs);
}

void print_jobs(struct Job arr[], int n){
    int i;
    for (i = 0; i < n; ++i){
        printf("%s %f %f\n", arr[i].PID, arr[i].T_gen, arr[i].T_comp);
    }
}

int main(int argc, char* argv[]){
    struct Job jobs_list[200]; //account for these lengths of input
    struct Process process_list[200];
    
    //if input has correct number of arguments
    if(argc != 3){
        printf("Invalid Input\n");
    }
    char* ptr1;
    double TsRR = strtod(strdup(argv[2]), &ptr1);
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
    
    run_experiments(jobs_list, num_jobs, TsRR);
    return 0;
}

