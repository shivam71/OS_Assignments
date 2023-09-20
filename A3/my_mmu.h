#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
// Include your Headers below


// You are not allowed to use the function malloc and calloc directly .
struct node{
	size_t size;
	struct* node next;
};
size_t initial_heap_size = 4096//bytes
struct* node head_ls  = mmap(NULL,initial_heap_size,PROT_READ|PROT_WRITE,MAP_ANON|MAP_PRIVATE,-1,0);
int start_vir_addr = head_ls;//check this// data type may give problem check int may not be good choice
int end_vir_addr = head_ls+4095;
int len_ls = 1;
size_t header_size = sizeof(node);
head_ls->size=initial_heap_size-header_size;
head_ls->next=NULL;
//Always maintain non empty free list 
// Function to allocate memory using mmap
bool do_split(size_t req_size,size_t block_size,size_t header_size){//criteria for splittinh



}

void* split(struct* node block_ptr,size_t req_size){

}


void delete_node(struct* node prev_ptr,struct* node curr_ptr){

}

void update_header(size_t size,struct* node next){


}


void* get_ptr_to_return(struct* node header_ptr){


}

void* get_header_ptr(struct* node ptr){


}

void insert_at_end(struct *node tail_ptr){



}

void* expand_heap(){



}

bool do_coalesce(struct *node prev,struct *node next){



}

void coalesce(struct *node prev,struct *node next){


}

void* my_malloc(size_t size) {
    // Able to find a free block of appropriate size
    // First fit 
    struct* node curr_ptr = head_ls;
    struct* node prev_ptr = NULL;
    size_t curr_size;
    void* return_ptr = NULL;
    while(curr_ptr!=NULL){
	curr_size = curr_ptr->size;
	if(size<=curr_size){
		if(do_split()){

			
		}else{
			// just delete the node 

		}
		break;
	}
	prev_ptr = curr_ptr;
	curr_ptr = curr_ptr->next;
    }
    // Need to expand the current heap 
    if(curr_ptr==NULL){
	    // expand the heap
	    // add the new part to the end using prev_ptr
	    // split the new part and allocate memory 
    }
    // free list became empty 
    if(len_ls==0){
	    // expand the heap 
	    // add new part to the memory 
    }
    return return_ptr;
}

// Function to allocate and initialize memory to zero using mmap
void* my_calloc(size_t nelem, size_t size) {
    // Your implementation of my_calloc goes here
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void* ptr) {
    // Your implementation of my_free goes here

}

void debug(){

}
