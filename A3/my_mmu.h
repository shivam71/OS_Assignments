#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include<unistd.h>
#include<stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
// Include your Headers below

// You are not allowed to use the function malloc and calloc directly .
struct node{
	size_t size;
	struct node* next;
} node_t;
size_t heap_size;
struct node* head_ls = NULL;
int len_ls = 1;
int magic_num = 12345;// check this
size_t header_size = sizeof(struct node);
void initialize(){
	heap_size = 4096;//bytes
	head_ls = (struct node*)mmap ( NULL, 4096,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
	head_ls->size=heap_size-header_size;
	head_ls->next=NULL;
	//Always maintain non empty free list
	// Function to allocate memory using mmap
}
void update_header(struct node* header_ptr,size_t size,struct node* next){
        header_ptr->next = next;
        header_ptr->size = size;
}


void* get_ptr_to_return(void* header_ptr){
        void* ptr;
        ptr= header_ptr+header_size;
        return ptr;

}

void* get_header_ptr(void* ptr){
        void* header_ptr;
        header_ptr= ptr-header_size;
        return header_ptr;
}
bool do_split(size_t req_size,size_t block_size){//criteria for splittinh
	if(req_size+header_size+32<block_size){
		return true;
	}else{
		return false;
	}
}

void* split(struct node* block_ptr,size_t req_size){
	size_t curr_size;
        curr_size = block_ptr->size;
	block_ptr->size=curr_size-(header_size+req_size);
	void* head_alloc = (void*)block_ptr;
       	head_alloc+=header_size;
	head_alloc+=(block_ptr->size);// Be careful
	update_header(head_alloc,req_size,magic_num);
	return get_ptr_to_return(head_alloc);
}


void delete_node(struct node* prev_ptr,struct node* curr_ptr){
	len_ls-=1;
	if(prev_ptr==NULL){//deleting the head of the free list 
		head_ls = curr_ptr->next;// head can become null 
	}else{
		prev_ptr->next = curr_ptr->next;
	}
}


struct node* expand_heap(size_t block_size){
	struct node* new_block_ptr;
        new_block_ptr= sbrk(block_size);
	heap_size+=block_size;
//	end_vir_addr+=(block_size);//check once
        update_header(new_block_ptr,block_size,NULL);
	len_ls+=1;
	return new_block_ptr;

}
/*
bool do_coalesce(struct *node prev,struct *node next){



}

void coalesce(struct *node prev,struct *node next){


}
*/
void* my_malloc(size_t size) {
    // Able to find a free block of appropriate size
    // First fit 
    if(head_ls==NULL){
	initialize();
    }
   
    struct node* curr_ptr;
    curr_ptr = head_ls;
    struct node* prev_ptr = NULL;
    size_t curr_size;
    void* return_ptr = NULL;
    while(curr_ptr!=NULL){
	curr_size = curr_ptr->size;
	if(size<=curr_size){
		if(do_split(size,curr_size)){
			return_ptr = split(curr_ptr,size);
		}else{
			// just delete the node
		       delete_node(prev_ptr,curr_ptr);
		       update_header(curr_ptr,curr_size,(struct node*)magic_num);
	       	       return_ptr = get_ptr_to_return(curr_ptr);	       
		}
		break;
	}else{
	    prev_ptr = curr_ptr;
    	    curr_ptr = curr_ptr->next;

	}
    }
    // Need to expand the current heap 
    if(curr_ptr==NULL){
	    // expand the heap // at least the request size 
	    // add the new part to the end using prev_ptr
	    // split the new part and allocate memory
	    if(size+header_size>heap_size){
		curr_ptr = expand_heap(size+header_size);
	    }else{
		curr_ptr = expand_heap(heap_size);
	    }
	    prev_ptr->next = curr_ptr;
	    curr_size = curr_ptr->size;
            if(do_split(size,curr_size)){
                        return_ptr = split(curr_ptr,size);
            }else{
                        // just delete the node
                       delete_node(prev_ptr,curr_ptr);
                       update_header(curr_ptr,curr_size,(struct node*)magic_num);
                       return_ptr = get_ptr_to_return(curr_ptr);
           }

    }
    // free list became empty 
    if(len_ls==0){
	    // expand the heap 
	    // add new part to the memory 
	    head_ls = expand_heap(heap_size);
    }
    return return_ptr;
}
/*
// Function to allocate and initialize memory to zero using mmap
void* my_calloc(size_t nelem, size_t size) {
    // Your implementation of my_calloc goes here
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void* ptr) {
    // Your implementation of my_free goes here

}

void debug(){

}*/
