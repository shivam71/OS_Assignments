#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

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

struct Process_Node{// use this implementing a 	queue using linked list  use for RR and MLFQ
	struct Process_Node* next;// pointer to the next Node in the list 
	struct Process* process;// pointer to the process associated with this node (could have this for Job <-> Process )
	double T_comp_left;//  Be careful while comparing double == 0  use double < epsilion 
};

struct Process_Queue{
	struct Process_Node* head;// deletion update this // create a function  
	struct Process_Node* tail;// insertion update this // create a function 
	int size;
};

void insert_queue(struct Process_Queue* queue,struct Process_Node* node){
	
}

void delete_queue(struct Process_Queue* queue){
	
}



/*void workload_resize(struct Job** wl,int new_size){
	wl = (struct Job**)realloc(wl,sizeof(struct Job*)*10);
}*/

double max_double(double first,double second){

	return (first>second) ? first : second;

}

double sample_exp(double exp_param){
	double prob;
	prob = rand()/(RAND_MAX+1.0);
	double u  = 1.0-prob;
	double log_val = log(u);
	return (-1.0)*log_val/exp_param;
}


void generate_wl(struct Job* wl,double exp_param,int num_jobs){
	wl[0].PID ="J0";
	wl[0].T_gen =0.0;
	wl[0].T_comp=sample_exp(exp_param);
	printf("%s  %g  %g\n",wl[0].PID,wl[0].T_gen,wl[0].T_comp);
	char* str_num = malloc(10);
	for(int idx=1;idx<num_jobs;idx++){		
		int len_req = snprintf(NULL,0,"%d",idx);
		str_num = (char *)realloc(str_num,len_req+2);
		str_num[0]='J';
        	snprintf(str_num+1,len_req+1,"%d",idx);
		wl[idx].PID = malloc(len_req+2);
		strcpy(wl[idx].PID,str_num);
		wl[idx].T_gen=(wl[idx-1].T_gen)+sample_exp(exp_param);
		wl[idx].T_comp=sample_exp(exp_param);
		printf("%s  %g  %g\n",wl[idx].PID,wl[idx].T_gen,wl[idx].T_comp);
	}
}



void print_process_exec_seq(struct CPU_burst* CPU_burst_ls,int num_bursts){
	for(int idx=0;idx<num_bursts;idx++){
		printf("%s %g %g ",CPU_burst_ls[idx].PID,CPU_burst_ls[idx].T_start,CPU_burst_ls[idx].T_end);
	}
	printf("\n");
}

void compute_print_metrics(struct Process* p_list, int num_p){
	// compute the avg RT and avg TT
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

void run_experiments(struct Job* wl,int num_jobs){
	run_FCFS(wl,num_jobs);
}

int main(){
	int num_wls = 5;
	int num_jobs[] ={1,2,3,4,5};
	double exp_params[] ={0.1,0.2,0.3,0.4,0.5};
	struct Job* workload = malloc(sizeof(struct Job)*1);
	for(int i=0;i<num_wls;i++){
		workload = (struct Job*)realloc(workload,sizeof(struct Job)*num_jobs[i]);
		printf("Generating Workload\n");
		generate_wl(workload,exp_params[i],num_jobs[i]);
		run_experiments(workload,num_jobs[i]);	
        }

	return 0;
}


