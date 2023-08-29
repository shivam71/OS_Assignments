#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

struct Job{
	char *PID;
	double T_gen;
	double T_comp;
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

int main(){
	int num_wls = 5;
	int num_jobs[] ={1,2,3,4,5};
	double exp_params[] ={0.1,0.2,0.3,0.4,0.5};
	struct Job* workload = malloc(sizeof(struct Job)*1);
	for(int i=0;i<num_wls;i++){
		workload = (struct Job*)realloc(workload,sizeof(struct Job)*num_jobs[i]);
		printf("Generating Workload\n");
		generate_wl(workload,exp_params[i],num_jobs[i]);	
        }

	return 0;
}


