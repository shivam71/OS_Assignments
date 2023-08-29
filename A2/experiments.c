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

/*void workload_resize(struct Job** wl,int new_size){
	wl = (struct Job**)realloc(wl,sizeof(struct Job*)*10);

}*/

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
		avg_TT+=p_list[idx].TAT;
	}
	avg_RT/=num_pr;
	avg_TT/=num_pr;
	printf("%g %g\n",avg_TT,avg_RT);
}


void run_FCFS(struct Job* wl){



}

void run_experiments(struct Job* wl){
	run_FCFS(wl);
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
		run_experiments(workload);	
        }

	return 0;
}


