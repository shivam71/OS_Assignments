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
	int curr_size = queue->size;
	if(curr_size==0){
		queue->head = node;// assuming node next is NULL
		queue->tail = node;
	}else{
		// insert at the end
		queue->tail->next = node;
		queue->tail = node;
	}
	queue->size+=1;	
}

void delete_queue(struct Process_Queue* queue){
	int curr_size = queue->size;
	if(curr_size==0){
		printf("Error queue is empty\n");
	}else{
		if(curr_size==1){
			queue->head = NULL;
			queue->tail = NULL;
		}else{
			queue->head = queue->head->next;// pointer assignment 
		}

		queue->size-=1;
	}
}



void add_job_to_plist(struct Job* wl,struct Process* p_list,int i){
		p_list[i].PID = wl[i].PID;
                p_list[i].T_gen = wl[i].T_gen;
                p_list[i].T_comp = wl[i].T_comp;
}
void update_process(struct Process* process){




}
void add_burst(struct CPU_burst* burst_ls,struct Process* pro_ptr,double start,double end,int num_bursts){



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
void run_RR(struct Job* wl,int num_jobs,double TS){
        double curr_time = 0.0;
	double next_time = 0.0;
        struct Process* p_list = malloc(sizeof(struct Process)*1);
        p_list = (struct Process*) realloc(p_list,sizeof(struct Process)*num_jobs);
        struct CPU_burst* CPU_burst_ls = malloc(sizeof(struct CPU_burst)*1);
	int num_bursts = 0;
        CPU_burst_ls =(struct CPU_burst*)realloc(CPU_burst_ls,sizeof(struct CPU_burst)*num_jobs);// at least this big
	int burst_ls_cap = num_jobs;// use this for reallocation if needed 
	struct Process_Queue pro_queue = {NULL,NULL,0};
        struct Process_Queue* queue_ptr = &pro_queue;
	struct Process_Node* p_node_ptr =NULL;
	struct Process* pro_ptr = NULL;
	int job_idx = 0;
	int queue_size =0;
	double t_left = 0.0;
	double job_t_comp= 0.0;
	while(job_idx<num_jobs){// Invariant all the jobs which have T_gen <= curr_time  they have been added to the queue at least once
		curr_time = wl[job_idx].T_gen;
		next_time = wl[job_idx].T_gen;
		while(next_time<=curr_time){
			// create a process node and add to the queue 
			// add to  the process list 
			add_job_to_p_list(wl,p_list,job_idx);
			struct Process_Node p_node = {NULL,p_list+job_idx,wl[job_idx].T_comp};
			insert_queue(queue_ptr,&p_node);
			job_idx+=1;
			if(job_idx=num_jobs){
				break;
			}else{
				next_time = wl[job_idx].T_gen;
			}

		}
		

		while((queue_ptr->size)>0){
			p_node_ptr = queue_ptr->head;
			pro_ptr = p_node_ptr->process;
			delete_queue(queue_ptr);
			t_left  = p_node_ptr->T_comp_left;
			job_t_comp = pro_ptr->T_comp;

			if(t_left==job_t_comp){// first schedule
				pro_ptr->T_first_sch = curr_time;	
			}

			if(t_left<=TS){
				// job completed
				add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left);// take care of context switch or not 
				curr_time+=t_left;
				update_process(p_node_ptr->process,curr_time);
			}else{
				// job not completed
				add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+TS);
				curr_time+=TS;
			}
			next_time = wl[job_idx].T_gen;
        	        while(next_time<=curr_time){
                        // create a process node and add to the queue
                        // add to  the process list
                        	add_job_to_p_list(wl,p_list,job_idx);
                        	struct Process_Node p_node = {NULL,p_list+job_idx,wl[job_idx].T_comp};
                       		insert_queue(queue_ptr,&p_node);
                        	job_idx+=1;
                        	if(job_idx=num_jobs){
                                	break;
                        	}else{
                                	next_time = wl[job_idx].T_gen;
                        	}

	                }
			if(t_left>TS){
				t_left-=TS;
				p_node_ptr->T_comp_left = t_left;
				insert_queue(queue_ptr,p_node_ptr);

			}
		}
		// When to add to process list when you add the job to the queue the first time 
		//  Add jobs to the queue either when the queue is empty or when a time slice is over or a process terminates 
		// When to add to CPU bursts when the context switch happens or when a process terminates 
		// When the process finishes in the TS alloted or earlier 
		// Update the process completion time in the process list 
	}
        printf("RR : ");
        print_process_exec_seq(CPU_burst_ls, num_bursts);
        compute_print_metrics(p_list, num_jobs);
        return;
}


void run_experiments(struct Job* wl,int num_jobs,double RR_TS){
	run_FCFS(wl,num_jobs);
	run_RR(wl,num_jobs,RR_TS);
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
		run_experiments(workload,num_jobs[i],5.0);	
        }

	return 0;
}


