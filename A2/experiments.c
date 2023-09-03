#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
FILE* fp;
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
	double T_comp_left; 
};

struct Process_Queue{
	struct Process_Node* head;// deletion update this // create a function  
	struct Process_Node* tail;// insertion update this // create a function 
	int size;
};
struct Process_PQueue{
	struct Process_Node** arr;// array of process node pointers
	int size;
	int capacity;
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

void swap(struct Process_Node** arr,int pos1,int pos2){
	struct Process_Node* temp = arr[pos1];
	arr[pos1] = arr[pos2];
	arr[pos2] = temp;
}


void heapify_up(struct Process_PQueue* pro_pq,int pos){
	if(pos==0) return;
	int size = pro_pq->size;
	// find the parent pos and compare with the parent and swap and call heapify_up on the parent 
	int par_pos = (pos-1)/2;
        double val_curr = pro_pq->arr[pos]->T_comp_left;
	double val_par = pro_pq->arr[par_pos]->T_comp_left;
	if(val_curr<val_par){
		swap(pro_pq->arr,pos,par_pos);
		heapify_up(pro_pq,par_pos);
	}else{
		return;
	}	

}
void heapify_down(struct Process_PQueue* pro_pq,int pos){
	// lexicographic is a problem -> Check 
        int size = pro_pq->size;// check this out 
        // find the parent pos and compare with the parent and swap and call heapify_up on the parent
        int lc_pos = (2*pos)+1;
	int rc_pos = (2*pos)+2;
	double val_curr = pro_pq->arr[pos]->T_comp_left;
	double val_lc;//pro_pq->arr[lc_pos]->T_comp_left;
	double val_rc;//pro_pq->arr[rc_pos]->T_comp_left;
	// check if they are within bounds
	if(lc_pos<size && rc_pos<size){
		val_lc = pro_pq->arr[lc_pos]->T_comp_left;
		val_rc = pro_pq->arr[rc_pos]->T_comp_left;
		if(val_lc<val_rc){
			if(val_curr>val_lc){
				swap(pro_pq->arr,pos,lc_pos);
				heapify_down(pro_pq,lc_pos);
			}
		}else{
			if(val_curr>val_rc){
				swap(pro_pq->arr,pos,rc_pos);
                                heapify_down(pro_pq,rc_pos);

			}
		}
	}else if(lc_pos<size){
		val_lc = pro_pq->arr[lc_pos]->T_comp_left;
		if(val_curr>val_lc){
                        swap(pro_pq->arr,pos,lc_pos);
                        heapify_down(pro_pq,lc_pos);
                }

	}else if(rc_pos<size){
		val_rc = pro_pq->arr[rc_pos]->T_comp_left;
		if(val_curr>val_rc){
                        swap(pro_pq->arr,pos,rc_pos);
                        heapify_down(pro_pq,rc_pos);

                }

	}else{
		return;
	}	
}

void insert_pq(struct Process_PQueue* pro_pq,struct Process_Node* node){
	int size = pro_pq->size;
	pro_pq->arr[size] = &(*node);
	// follow the heapify up // handle cases when it was empty
	pro_pq->size = size+1;
	if(size>0){
		heapify_up(pro_pq,size);
	}	
}
struct Process_Node* extract_pq(struct Process_PQueue* pro_pq){
	int size = pro_pq->size;
	struct Process_Node* p_node = &(*(pro_pq->arr[0]));
	pro_pq->arr[0] = &(*(pro_pq->arr[size-1]));// move the last to first
	pro_pq->size-=1;
	// follow the heapify down // handle cases when it becomes empty 
	if(size>1){
		heapify_down(pro_pq,0);
	}
	return p_node;
}
void priority_boost(struct Process_Queue* queue_arr){
	int size;
	struct Process_Node* node_ptr = NULL;
	for(int q_idx =1;q_idx<=2;q_idx++){
		size = queue_arr[q_idx].size;
		for(int idx=0;idx<size;idx++){
			node_ptr = queue_arr[q_idx].head;
			delete_queue(&queue_arr[q_idx]);
			insert_queue(&queue_arr[0],node_ptr);
		}
	}
}
void init_queues(struct Process_Queue* queue_arr){
	for(int idx=0;idx<3;idx++){
		queue_arr[idx].size=0;
		queue_arr[idx].head=NULL;
		queue_arr[idx].tail=NULL;
	}
}

void add_job_to_plist(struct Job* wl,struct Process* p_list,int i){
		p_list[i].PID = wl[i].PID;
                p_list[i].T_gen = wl[i].T_gen;
                p_list[i].T_comp = wl[i].T_comp;
}

void update_process(struct Process* pro_ptr,double T_fin){
		pro_ptr->T_finish = T_fin;
		pro_ptr->RT = (pro_ptr->T_first_sch)-(pro_ptr->T_gen);
		pro_ptr->TAT=T_fin-(pro_ptr->T_gen);

}

struct CPU_burst* add_burst(struct CPU_burst* burst_ls,struct Process* pro_ptr,double start,double end,int* num_bursts, int* ls_cap){
	if(*num_bursts==0){
		burst_ls[0].PID = pro_ptr->PID;
		burst_ls[0].T_start = start;
		burst_ls[0].T_end = end;
		*num_bursts=1;
	}else{
		// check if a new CPU burst needs to be created accordingly 
		// also check if reallocation is required
		int curr_num_bursts = *num_bursts;
		if(strcmp(burst_ls[curr_num_bursts-1].PID,pro_ptr->PID)==0){
			// no context switch 
		
			burst_ls[curr_num_bursts-1].T_end=end;
		}else{
			// new bursti
			int curr_cap = *ls_cap;
			if(curr_num_bursts==curr_cap){
				burst_ls = (struct CPU_burst*)realloc(burst_ls,sizeof(struct CPU_burst)*curr_num_bursts*2);
				*ls_cap= curr_num_bursts*2;
			}
			burst_ls[curr_num_bursts].PID=pro_ptr->PID;
			burst_ls[curr_num_bursts].T_start=start;
			burst_ls[curr_num_bursts].T_end= end;
			*num_bursts = curr_num_bursts+1;
	}
	}
	return burst_ls;
}


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
double sample_uniform(double min,double max){
        double prob;
        prob = rand()/(RAND_MAX+1.0);
        double x = min+((max-min)*(prob));//using inverse CDF
        return x;
}

void generate_wl(struct Job* wl,double exp_param,double uni_a,double uni_b,int num_jobs){
	wl[0].PID ="J0";
	wl[0].T_gen =0.0;
	wl[0].T_comp=sample_uniform(uni_a,uni_b);
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
		wl[idx].T_comp=sample_uniform(uni_a,uni_b);
		printf("%s  %g  %g\n",wl[idx].PID,wl[idx].T_gen,wl[idx].T_comp);
	}
}



void print_process_exec_seq(struct CPU_burst* CPU_burst_ls,int num_bursts){
	for(int idx=0;idx<num_bursts;idx++){
		printf("%s %0.3f %0.3f ",CPU_burst_ls[idx].PID,CPU_burst_ls[idx].T_start,CPU_burst_ls[idx].T_end);
		fprintf(fp,"%s %0.3f %0.3f ",CPU_burst_ls[idx].PID,CPU_burst_ls[idx].T_start,CPU_burst_ls[idx].T_end);
	}
	printf("\n");
	fprintf(fp,"\n");
	free(CPU_burst_ls);
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
	printf("%0.3f %0.3f\n",avg_TAT,avg_RT);
	fprintf(fp,"%0.3f %0.3f\n",avg_TAT,avg_RT);
	free(p_list);
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
void run_SJF(struct Job* wl,int num_jobs){
	double curr_time = 0.0;
        double next_time = 0.0;
        struct Process* p_list = malloc(sizeof(struct Process)*1);
        p_list = (struct Process*) realloc(p_list,sizeof(struct Process)*num_jobs);
        struct CPU_burst* CPU_burst_ls = malloc(sizeof(struct CPU_burst)*1);
        int num_bursts = 0;
        CPU_burst_ls =(struct CPU_burst*)realloc(CPU_burst_ls,sizeof(struct CPU_burst)*num_jobs);// at least this big
        int burst_ls_cap = num_jobs;// use this for reallocation if needed
        struct Process_PQueue pro_pq;// insert_pq extract_pq
        pro_pq.size = 0;
        pro_pq.capacity = num_jobs;
        pro_pq.arr = (struct Process_Node**)malloc(sizeof(struct Process_Node*)*num_jobs);
        struct Process_PQueue* pqueue_ptr = &pro_pq;
        struct Process_Node* p_node_ptr =NULL;
        struct Process* pro_ptr = NULL;
        struct Process_Node* p_node_ptr_new =NULL;
        int job_idx = 0;
        int pqueue_size =0;
        double TS;
        double t_left = 0.0;
        double job_t_comp= 0.0;
	while(job_idx<num_jobs){
                curr_time = wl[job_idx].T_gen;
                next_time = wl[job_idx].T_gen;
                while(next_time<=curr_time){
                        // create a process node and add to the queue
                        // add to  the process list
                                add_job_to_plist(wl,p_list,job_idx);
                                p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
                                p_node_ptr_new->next= NULL;
                                p_node_ptr_new->process = p_list+job_idx;
                                p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                                insert_pq(pqueue_ptr,p_node_ptr_new);
                                job_idx+=1;
                                if(job_idx==num_jobs){
                                        break;
                                }else{
                                        next_time = wl[job_idx].T_gen;
                                }

               }
	       while(pro_pq.size>0){
                        p_node_ptr = extract_pq(pqueue_ptr);// front and pop
                        pro_ptr = p_node_ptr->process;
                        t_left  = p_node_ptr->T_comp_left;
                        job_t_comp = pro_ptr->T_comp;
                        pro_ptr->T_first_sch = curr_time;
                        CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not
                        curr_time+=t_left;
                        update_process(p_node_ptr->process,curr_time);
			if(job_idx<num_jobs){
                        	next_time = wl[job_idx].T_gen;
                        	while(next_time<=curr_time){
                                	add_job_to_plist(wl,p_list,job_idx);
                                	p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
                                	p_node_ptr_new->next= NULL;
                                	p_node_ptr_new->process = p_list+job_idx;
                                	p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                                	insert_pq(pqueue_ptr,p_node_ptr_new);
                                	job_idx+=1;
                                	if(job_idx==num_jobs){
                                        	break;
                                	}else{
                                        	next_time = wl[job_idx].T_gen;
                                	}

                        	}
                        }

      		}

	}
        
        printf("SJF : ");
        print_process_exec_seq(CPU_burst_ls, num_bursts);
        compute_print_metrics(p_list, num_jobs);
        return;

}
void run_SCTF(struct Job* wl,int num_jobs){
	double curr_time = 0.0;
        double next_time = 0.0;
        struct Process* p_list = malloc(sizeof(struct Process)*1);
        p_list = (struct Process*) realloc(p_list,sizeof(struct Process)*num_jobs);
        struct CPU_burst* CPU_burst_ls = malloc(sizeof(struct CPU_burst)*1);
        int num_bursts = 0;
        CPU_burst_ls =(struct CPU_burst*)realloc(CPU_burst_ls,sizeof(struct CPU_burst)*num_jobs);// at least this big
        int burst_ls_cap = num_jobs;// use this for reallocation if needed
        struct Process_PQueue pro_pq;// insert_pq extract_pq
        pro_pq.size = 0;
	pro_pq.capacity = num_jobs;
	pro_pq.arr = (struct Process_Node**)malloc(sizeof(struct Process_Node*)*num_jobs);
        struct Process_PQueue* pqueue_ptr = &pro_pq;
        struct Process_Node* p_node_ptr =NULL;
        struct Process* pro_ptr = NULL;
        struct Process_Node* p_node_ptr_new =NULL;
        int job_idx = 0;
        int pqueue_size =0;
	double TS;
        double t_left = 0.0;
        double job_t_comp= 0.0;
	while(job_idx<num_jobs){
		curr_time = wl[job_idx].T_gen;
                next_time = wl[job_idx].T_gen;
                while(next_time<=curr_time){
                        // create a process node and add to the queue
                        // add to  the process list
                                add_job_to_plist(wl,p_list,job_idx);
                                p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
                                p_node_ptr_new->next= NULL;
                                p_node_ptr_new->process = p_list+job_idx;
                                p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                                insert_pq(pqueue_ptr,p_node_ptr_new);
                                job_idx+=1;
                                if(job_idx==num_jobs){
                                        break;
                                }else{
                                        next_time = wl[job_idx].T_gen;
                                }

               }
	       while(pro_pq.size>0){
			p_node_ptr = extract_pq(pqueue_ptr);// front and pop 
			TS = next_time-curr_time;
			if(job_idx==num_jobs){
				TS = p_node_ptr->T_comp_left;// current job goes till completion 
			}
			pro_ptr = p_node_ptr->process;
                        t_left  = p_node_ptr->T_comp_left;
                        job_t_comp = pro_ptr->T_comp;
                    
                        if(t_left==job_t_comp){// first schedule
                                pro_ptr->T_first_sch = curr_time;
                        }
                        if(t_left<=TS){
                                // job completed
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not
                                curr_time+=t_left;
                                update_process(p_node_ptr->process,curr_time);
                        }else{
                                // job not completed
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+TS,&num_bursts,&burst_ls_cap);

                                curr_time+=TS;
                        }
			if(job_idx<num_jobs){
                        next_time = wl[job_idx].T_gen;
                        while(next_time<=curr_time){
                        // create a process node and add to the queue
                        // add to  the process list
                                add_job_to_plist(wl,p_list,job_idx);
                                p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
                                p_node_ptr_new->next= NULL;
                                p_node_ptr_new->process = p_list+job_idx;
                                p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                                insert_pq(pqueue_ptr,p_node_ptr_new);
                                job_idx+=1;
                                if(job_idx==num_jobs){
                                        break;
                                }else{
                                        next_time = wl[job_idx].T_gen;
                                }

                        }
                        }
                        if(t_left>TS){
                                t_left-=TS;
                                p_node_ptr->T_comp_left = t_left;
                                insert_pq(pqueue_ptr,p_node_ptr);

                        }

	       }
	}
	printf("SCTF : ");
        print_process_exec_seq(CPU_burst_ls, num_bursts);
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
	struct Process_Node* p_node_ptr_new =NULL; 
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
                                add_job_to_plist(wl,p_list,job_idx);
                                p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
                                p_node_ptr_new->next= NULL;
                                p_node_ptr_new->process = p_list+job_idx;
                                p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                                insert_queue(queue_ptr,p_node_ptr_new);
                                job_idx+=1;
                                if(job_idx==num_jobs){
                                        break;
                                }else{
                                        next_time = wl[job_idx].T_gen;
                                }

                }
		while((queue_ptr->size)>0){
			p_node_ptr = &(*queue_ptr->head);
			pro_ptr = p_node_ptr->process;
			t_left  = p_node_ptr->T_comp_left;
			job_t_comp = pro_ptr->T_comp;
			delete_queue(queue_ptr);
			if(t_left==job_t_comp){// first schedule
				pro_ptr->T_first_sch = curr_time;	
			}
			if(t_left<=TS){
				// job completed
				CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not 
				curr_time+=t_left;
				update_process(p_node_ptr->process,curr_time);
			}else{
				// job not completed
				CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+TS,&num_bursts,&burst_ls_cap);
			
				curr_time+=TS;
			}
			if(job_idx<num_jobs){
			next_time = wl[job_idx].T_gen;
        	        while(next_time<=curr_time){
                        // create a process node and add to the queue
                        // add to  the process list
                        	add_job_to_plist(wl,p_list,job_idx);
                        	p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
				p_node_ptr_new->next= NULL;
				p_node_ptr_new->process = p_list+job_idx;
				p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                       		insert_queue(queue_ptr,p_node_ptr_new);
                        	job_idx+=1;
                        	if(job_idx==num_jobs){
                                	break;
                        	}else{
                                	next_time = wl[job_idx].T_gen;
                        	}

	                }
			}
			if(t_left>TS){
				t_left-=TS;
				p_node_ptr->T_comp_left = t_left;
				insert_queue(queue_ptr,p_node_ptr);

			}
		}
	}
        printf("RR : ");
        print_process_exec_seq(CPU_burst_ls, num_bursts);
        compute_print_metrics(p_list, num_jobs);
        return;
}

void run_MLFQ(struct Job* wl,int num_jobs,double Q1_TS,double Q2_TS,double Q3_TS,double T_PB){
        // Array of queue ptrs
	// Array of TS
	// Should we stop the execution once the curr time is T_PB
	//  curr_time > T_PB then PB similarly next -> curr_time+T_PB
	//  Time for the next boost -> Stored 
	// when we do PB -> How do we put the things in the Top queue -> Assume that 2nd queue then 3rd queue 
	//  Add the jobs which have arrived during a time slice after the time slice into the first queue 
	struct Process* p_list = malloc(sizeof(struct Process)*1);
        p_list = (struct Process*) realloc(p_list,sizeof(struct Process)*num_jobs);
        struct CPU_burst* CPU_burst_ls = malloc(sizeof(struct CPU_burst)*1);
        int num_bursts = 0;
        CPU_burst_ls =(struct CPU_burst*)realloc(CPU_burst_ls,sizeof(struct CPU_burst)*num_jobs);// at least this big
        int burst_ls_cap = num_jobs;// use this for reallocation if needed
	double TS_ls[] = {Q1_TS,Q2_TS,Q3_TS};
	double TS = 0.0;
	struct Process_Queue queue_arr[3];
	init_queues(queue_arr);
	struct 	Process_Node* p_node_ptr;
	struct Process_Queue* queue_ptr;
	struct Process_Node* p_node_ptr_new;
	struct Process* pro_ptr = NULL;
	int job_idx = 0;
	int q_idx = 0;
	int size_q1,size_q2,size_q3,curr_size;
	double curr_time = 0.0;
	double next_time,t_left,job_t_comp;
	double next_PB = T_PB;// if all queues become empty then update the next_PB time 
	while(job_idx<num_jobs){
		// add jobs if all queues are empty
		
		curr_time = wl[job_idx].T_gen;
		next_PB = curr_time+T_PB;// Cannot boost priority for empty queue 
                next_time = wl[job_idx].T_gen;
                while(next_time<=curr_time){
                        // create a process node and add to the queue
                        // add to  the process list
				q_idx=0;//added new jobs 
                                add_job_to_plist(wl,p_list,job_idx);
                                p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
                                p_node_ptr_new->next= NULL;
                                p_node_ptr_new->process = p_list+job_idx;
                                p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                                insert_queue(&queue_arr[0],p_node_ptr_new);// always add jobs to the top queue
                                job_idx+=1;
                                if(job_idx==num_jobs){
                                        break;
                                }else{
                                        next_time = wl[job_idx].T_gen;
                                }

                }
		q_idx = 0;// whenever you add jobs or PBoost  
		size_q1 = queue_arr[0].size;
		size_q2 = queue_arr[1].size;
		size_q3= queue_arr[2].size;
		while(size_q1>0 || size_q2>0 || size_q3>0){// change the queue we are doing RR on currently q_idx
			curr_size = queue_arr[q_idx].size;
			queue_ptr = &queue_arr[q_idx];
			TS = TS_ls[q_idx];
			p_node_ptr = &(*queue_ptr->head);
                       	pro_ptr = p_node_ptr->process;
                        t_left  = p_node_ptr->T_comp_left;
                        job_t_comp = pro_ptr->T_comp;
                        delete_queue(queue_ptr);
                        if(t_left==job_t_comp){// first schedule
                                pro_ptr->T_first_sch = curr_time;
                        }
                        if(t_left<=TS){
                                // job completed
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not
                                curr_time+=t_left;
                                update_process(p_node_ptr->process,curr_time);
                        }else{
                                // job not completed
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+TS,&num_bursts,&burst_ls_cap);

                                curr_time+=TS;
                        }
                        if(job_idx<num_jobs){
                        next_time = wl[job_idx].T_gen;
			 while(next_time<=curr_time){
                        // create a process node and add to the queue
                        // add to  the process list
                               // update q_idx
                                add_job_to_plist(wl,p_list,job_idx);
                                p_node_ptr_new = (struct Process_Node*)malloc(sizeof(struct Process_Node));
                                p_node_ptr_new->next= NULL;
                                p_node_ptr_new->process = p_list+job_idx;
                                p_node_ptr_new->T_comp_left = wl[job_idx].T_comp;
                                insert_queue(&queue_arr[0],p_node_ptr_new);// always add jobs to the top queue
                                job_idx+=1;
                                if(job_idx==num_jobs){
                                        break;
                                }else{
                                        next_time = wl[job_idx].T_gen;
                                }

               		   }
			}
			if(t_left>TS){
                                t_left-=TS;
                                p_node_ptr->T_comp_left = t_left;
				if(q_idx==2){// cannot further lower the priority 
                                	insert_queue(queue_ptr,p_node_ptr);
				}else{
					insert_queue(&queue_arr[q_idx+1],p_node_ptr);// lower priority 
				}
                        }
			if(curr_time>=next_PB){
				next_PB=curr_time+T_PB;// change this one as well
				priority_boost(queue_arr);
			}
			// update the q_idx
			size_q1 = queue_arr[0].size;
                        size_q2 = queue_arr[1].size;
                        size_q3= queue_arr[2].size;


			if(size_q1>0){
				q_idx=0;
			}else if(size_q2>0){
				q_idx=1;
			}else if(size_q3>0){
				q_idx=2;
			}else{
				break;
			}
			
		    

		}
	}
	printf("MLFQ : ");
        print_process_exec_seq(CPU_burst_ls, num_bursts);
        compute_print_metrics(p_list, num_jobs);
        return; 
}


void run_experiments(struct Job* wl,int num_jobs,double RR_TS,double Q1_TS,double Q2_TS,double Q3_TS,double T_PB){
	run_FCFS(wl,num_jobs);
	run_RR(wl,num_jobs,RR_TS);
	run_SJF(wl,num_jobs);
        run_SCTF(wl,num_jobs);
	run_MLFQ(wl,num_jobs,Q1_TS,Q2_TS,Q3_TS,T_PB);
}

int main(){
	
	int num_wls = 5;
	int num_jobs[] ={10,10,10,10,10};
	int num_params_set = 4;
	double RR_TS[] = {1.0,2.0,3.0,4.0};
	double Q1_TS[] = {2.0,3.0,4.0,5.0};
	double Q2_TS[] = {4.0,6.0,8.0,10.0};
	double Q3_TS[] = {8.0,12.0,16.0,14.0};
	double T_PB[] = {8.0,12.0,10.0,10.0};
	double exp_params[] ={0.05,0.1,0.2,0.4,0.8};//rate parameter
	double uniform_a[] ={10.0,1.0,10.0,20.0};
	double uniform_b[] ={10.0,10.0,100.0,30.0};
	struct Job* workload = malloc(sizeof(struct Job)*1);
	char* name_out_file = NULL;
	for(int i=0;i<num_wls;i++){
		workload = (struct Job*)realloc(workload,sizeof(struct Job)*num_jobs[i]);
		printf("Generating Workload\n");
		generate_wl(workload,exp_params[i],uniform_a[i],uniform_b[i],num_jobs[i]);
		for(int j =0;j<num_params_set;j++){
			int exp_num = (i*num_params_set)+j;
			int len_req = snprintf(NULL,0,"%d",exp_num);
			name_out_file = realloc(name_out_file,len_req+17);
			snprintf(name_out_file,len_req+17,"exp_%d_outputs.txt",exp_num);
			fp = fopen(name_out_file,"w");
			run_experiments(workload,num_jobs[i],RR_TS[j],Q1_TS[j],Q2_TS[j],Q3_TS[j],T_PB[j]);
			fclose(fp);
		}
        }
	workload = (struct Job*)malloc(sizeof(struct Job)*3);
	workload[0].PID ="Job1";
	workload[1].PID ="Job2";
	workload[2].PID ="Job3";
	workload[0].T_gen =0.0;
        workload[1].T_gen =2.0;
        workload[2].T_gen =2.0;
	workload[0].T_comp =18.0;
        workload[1].T_comp =7.0;
        workload[2].T_comp =10.0;
	run_experiments(workload,3,5.0,5.0,10.0,15.0,100.0);
	return 0;
}


