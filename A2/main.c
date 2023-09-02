#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

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
    //printf("Adding bursts\n");
    if(*num_bursts==0){
        burst_ls[0].PID = pro_ptr->PID;
        burst_ls[0].T_start = start;
        burst_ls[0].T_end = end;
        *num_bursts=1;
    }else{
        // check if a new CPU burst needs to be created accordingly
        // also check if reallocation is required
    //    printf("Problem\n");
        int curr_num_bursts = *num_bursts;
        if(strcmp(burst_ls[curr_num_bursts-1].PID,pro_ptr->PID)==0){
            // no context switch
        
            burst_ls[curr_num_bursts-1].T_end=end;
        }else{
            // new bursti
    //        printf("Problem\n");
            int curr_cap = *ls_cap;
//            printf("%d  %d\n",curr_num_bursts,curr_cap);
            if(curr_num_bursts==curr_cap){
                burst_ls = (struct CPU_burst*)realloc(burst_ls,sizeof(struct CPU_burst)*curr_num_bursts*2);
                *ls_cap= curr_num_bursts*2;
            }
    //        printf("Adding bursts\n");
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

double min_double(double first,double second){
    return (first<second) ? first : second;
}

int job_comparator(const void *v1, const void *v2){
    const struct Job *p1 = (struct Job *)v1;
    const struct Job *p2 = (struct Job *)v2;
    if(p1->T_gen > p2->T_gen)
        return 1;
    else if(p1->T_gen < p2->T_gen)
        return -1;
    else if(strcmp(p1->PID, p2->PID) >= 0)
        return 1;
    return -1;
}

void print_process_exec_seq(struct CPU_burst* CPU_burst_ls,int num_bursts){
    for(int idx=0;idx<num_bursts;idx++){
        fprintf(fp, "%s %g %g ",CPU_burst_ls[idx].PID,CPU_burst_ls[idx].T_start,CPU_burst_ls[idx].T_end);
    }
    fprintf(fp, "\n");
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
    fprintf(fp, "%g %g\n",avg_TAT,avg_RT);
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
        fprintf(fp, "FCFS : ");
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
                                // job completed
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not
                                curr_time+=t_left;
                                update_process(p_node_ptr->process,curr_time);
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
        //              printf("Here\n");
                        }

                   }

               }
        
        fprintf(fp, "SJF : ");
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
                      //printf("t_left before %g\n",t_left);
                        job_t_comp = pro_ptr->T_comp;
                    
                      //printf("PID %s\n",p_node_ptr->process->PID);
                        if(t_left==job_t_comp){// first schedule
                                pro_ptr->T_first_sch = curr_time;
                        }
        //              printf("Here\n");
                        if(t_left<=TS){
                                // job completed
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not
                                curr_time+=t_left;
                                update_process(p_node_ptr->process,curr_time);
                        }else{
                                // job not completed
        //                      printf("Reached here\n");
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
        //              printf("Here\n");
                        }
                        if(t_left>TS){
                                t_left-=TS;
        //                      printf("PID %s %g\n",p_node_ptr->process->PID,p_node_ptr->T_comp_left);
                                p_node_ptr->T_comp_left = t_left;
                                insert_pq(pqueue_ptr,p_node_ptr);
        //                      printf("t_left %g\n",t_left);
        //                      printf("t_left 2 %g\n",queue_ptr->tail->T_comp_left);

                        }

           }
    }
    fprintf(fp, "SCTF : ");
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
    //        printf("PID %s",p_node_ptr->process->PID);
            pro_ptr = p_node_ptr->process;
            t_left  = p_node_ptr->T_comp_left;
    //        printf("t_left before %g\n",t_left);
            job_t_comp = pro_ptr->T_comp;
            delete_queue(queue_ptr);
    //        printf("PID after deletion %s\n",p_node_ptr->process->PID);
            if(t_left==job_t_comp){// first schedule
                pro_ptr->T_first_sch = curr_time;
            }
    //        printf("Here\n");
            if(t_left<=TS){
                // job completed
                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not
                curr_time+=t_left;
                update_process(p_node_ptr->process,curr_time);
            }else{
                // job not completed
    //            printf("Reached here\n");
                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+TS,&num_bursts,&burst_ls_cap);
            
                curr_time+=TS;
            }
    //        printf("Here again\n");
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
    //        printf("Here\n");
            }
            if(t_left>TS){
                t_left-=TS;
    //            printf("PID %s %g\n",p_node_ptr->process->PID,p_node_ptr->T_comp_left);
                p_node_ptr->T_comp_left = t_left;
                insert_queue(queue_ptr,p_node_ptr);
    //            printf("t_left %g\n",t_left);
    //            printf("t_left 2 %g\n",queue_ptr->tail->T_comp_left);

            }
        }
        // When to add to process list when you add the job to the queue the first time
        //  Add jobs to the queue either when the queue is empty or when a time slice is over or a process terminates
        // When to add to CPU bursts when the context switch happens or when a process terminates
        // When the process finishes in the TS alloted or earlier
        // Update the process completion time in the process list
    }
        fprintf(fp, "RR : ");
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
    struct     Process_Node* p_node_ptr;
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
            //printf("q_idx %d size %d",q_idx,queue_arr[q_idx].size);
            curr_size = queue_arr[q_idx].size;
            queue_ptr = &queue_arr[q_idx];
            TS = TS_ls[q_idx];
            //while(curr_size>0){
                // RR on the queue with q_idx
                // Exit this if time is more than the next PB
                // Exit this if any new process has arrived
            //    printf("Queue empty %d",queue_ptr->size);
                   p_node_ptr = &(*queue_ptr->head);
                      //printf("PID %s\n",p_node_ptr->process->PID);
                           pro_ptr = p_node_ptr->process;
                        t_left  = p_node_ptr->T_comp_left;
                      //printf("t_left before %g\n",t_left);
                        job_t_comp = pro_ptr->T_comp;
                        delete_queue(queue_ptr);
                      //printf("PID after deletion %s\n",p_node_ptr->process->PID);
                        if(t_left==job_t_comp){// first schedule
                                pro_ptr->T_first_sch = curr_time;
                        }
                      //printf("Here\n");
                        if(t_left<=TS){
                                // job completed
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+t_left,&num_bursts,&burst_ls_cap);// take care of context switch or not
                                curr_time+=t_left;
                                update_process(p_node_ptr->process,curr_time);
                        }else{
                                // job not completed
                        //      printf("Reached here\n");
                                CPU_burst_ls = add_burst(CPU_burst_ls,pro_ptr,curr_time,curr_time+TS,&num_bursts,&burst_ls_cap);

                                curr_time+=TS;
                        }
                      //printf("Here again\n");
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
                        //      printf("PID %s %g\n",p_node_ptr->process->PID,p_node_ptr->T_comp_left);
                                p_node_ptr->T_comp_left = t_left;
                if(q_idx==2){// cannot further lower the priority
                                    insert_queue(queue_ptr,p_node_ptr);
                }else{
                    insert_queue(&queue_arr[q_idx+1],p_node_ptr);// lower priority
                }
                          //    printf("t_left %g\n",t_left);
                  
                        }



            // when to do a p boost
            // Check if p boost is required then do it should I boost priority if everything has the highest priority
             
            // When should the next p_boost be done
            //printf("finding size\n");
            if(curr_time>=next_PB){
                next_PB=curr_time+T_PB;// change this one as well
                priority_boost(queue_arr);
            }
            // update the q_idx
            //printf("Size found\n");
            size_q1 = queue_arr[0].size;
                        size_q2 = queue_arr[1].size;
                        size_q3= queue_arr[2].size;


            if(size_q1>0){
                q_idx=0;
            }else if(size_q2>0){
                q_idx=1;
            //    printf("queue 2 non empty\n");
            }else if(size_q3>0){
                q_idx=2;
            }else{
                break;
            }
            
            

        }
    }
    fprintf(fp, "MLFQ : ");
        print_process_exec_seq(CPU_burst_ls, num_bursts);
        compute_print_metrics(p_list, num_jobs);
        return;
}

void run_experiments(struct Job* wl,int num_jobs,double RR_TS,double Q1_TS,double Q2_TS,double Q3_TS,double T_PB){
    run_FCFS(wl,num_jobs);
    run_SJF(wl,num_jobs);
    run_SCTF(wl,num_jobs);
    run_RR(wl,num_jobs,RR_TS);
    run_MLFQ(wl,num_jobs,Q1_TS,Q2_TS,Q3_TS,T_PB);
}

void print_jobs(struct Job arr[], int n){
    int i;
    for (i = 0; i < n; ++i){
        fprintf(fp, "%s %f %f\n", arr[i].PID, arr[i].T_gen, arr[i].T_comp);
    }
}

int main(int argc, char* argv[]){
    struct Job jobs_list[32768]; //account for these lengths of input
    struct Process process_list[32768];
    
    //if input has correct number of arguments
    if(argc != 8){
        printf("Invalid Input\n");
    }
    char* ptr1;
    double RR_TS = strtod(strdup(argv[3]), &ptr1);
    double Q1_TS = strtod(strdup(argv[4]), &ptr1);
    double Q2_TS = strtod(strdup(argv[5]), &ptr1);
    double Q3_TS = strtod(strdup(argv[6]), &ptr1);
    double T_PB = strtod(strdup(argv[7]), &ptr1);
    //reading from input .txt file
    fp = fopen(argv[1], "r");
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
    //print_jobs(jobs_list, num_jobs);
    
    FILE* fp = fopen(argv[2], "w");
    run_experiments(jobs_list, num_jobs, RR_TS, Q1_TS, Q2_TS, Q3_TS, T_PB);
    fclose(fp);
    return 0;
}
