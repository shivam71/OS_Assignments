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
};
size_t heap_size;
void* heap_start_ptr,heap_end_ptr;
struct node* head_ls = NULL;
int len_ls = 0;// Check that whenever we modify the free list that we change the size and the header 
int num_alloc_blocks = 0;
int magic_num = 12345;// check this
size_t header_size = sizeof(struct node);

void initialize(){
	heap_size = 4096;//bytes
	head_ls = (struct node*)mmap ( NULL, heap_size,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
	head_ls->size=heap_size-header_size;
	head_ls->next=NULL;
	heap_start_ptr = head_ls;
	heap_end_ptr = head_ls+heap_size-1;
	len_ls=1;
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
	heap_end_ptr+=block_size;
	len_ls+=1;
	return new_block_ptr;

}

bool do_coalesce(struct *node prev,struct *node next){
	size_t prev_size = prev->size;
	prev = (void*) prev;
	next = (void*) next;
	prev+=(header_size+prev_size);
	return (prev==next);
}

void coalesce(struct *node prev,struct *node next){
	// condition that we checked for do coalesce already holds
	len_ls-=1;// IMP don't forget 
	size_t total_free_size = (prev->size)+(header_size)+(next->size);
	prev->next = next->next;
	prev->size = total_free_size;
}

void insert_free_ls(struct node* ptr,struct node* prev_ptr,struct node* next_ptr){
	// don't forget to  change the size of the free list and the header 
	// handle the edge cases where the prev or the next is NULL
	if(prev_ptr==NULL){
		ptr->next = next_ptr;
		head_ls = ptr;
	}else if(next_ptr==NULL){// insert at the end
		prev_ptr->next = ptr;
		ptr->next =NULL;// IMP
        }else{
		prev_ptr->next = ptr;
		ptr->next = next_ptr;
       }
       // update the size
       len_ls+=1;
}

void* my_malloc(size_t size) {
    // Able to find a free block of appropriate size
    // First fit 
    if(head_ls==NULL){
	initialize();
    }
    num_alloc_blocks+=1;
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

// Function to allocate and initialize memory to zero using mmap
void* my_calloc(size_t nelem, size_t size) {
    // Your implementation of my_calloc goes here
    size_t num_bytes = nelem*size;
    unsigned char* ptr = my_malloc(num_bytes);
    for(int idx=0;idx<num_bytes;idx++){
	ptr[idx] = 0;
    }
    return (void*) ptr;
}

// Function to release memory allocated using my_malloc and my_calloc
void my_free(void* ptr) {
    // Your implementation of my_free goes here
    // initialize the variables
    // iterate to find the position to insert 
    //  insert 
    //  coalesce 
    num_alloc_blocks-=1;
    void* node next_ptr = head_ls;
    void* node prev_ptr = NULL;
    ptr = get_header_ptr(ptr);
    while(next_ptr!=NULL){
		if(ptr<next_ptr){// check if the type of the pointer matter while doing pointer comparison 
			break;
		}else{
			continue;
               }
	       prev_ptr = next_ptr;
	       next_ptr = (void*) (((struct node*)next_ptr)->next)
    }
    insert_free_ls(ptr,prev_ptr,next_ptr);
    if(next_ptr!=NULL){
        if(do_coalesce(ptr,next_ptr)){
		coalesce(ptr,next_ptr);
	}
    }
    if(prev_ptr!=NULL){
	if(do_coalesce(prev_ptr,ptr)){
                coalesce(prev_ptr,ptr);
        }
    }
}

void* realloc(void* ptr,size_t size){
	unsigned char* ptr_return = my_malloc(size);// copy bytes using unsigned char vs char test by storing negative numbers array and explore
	unsigned char* ptr_given =  (unsigned char*) ptr;
	for(int idx = 0;idx<size;idx++){
		ptr_return[idx]=ptr_given[idx];
	}
	my_free(ptr);
	return (void*)ptr_return;
}

void debug(){
	printf("Start addr of the heap %p\n",heap_start_ptr);
	printf("End addr of the heap %p\n",heap_end_ptr);
	printf("Addr of the free list head %p\n",head_ls);
	printf("Length of the free list %d\n",len_ls);
        printf("Number of allocated blocks %d\n",num_alloc_blocks);
	void* ptr_raw;
	void* prev_block_ptr = heap_start_ptr-header_size;
	struct node* ptr_node;
	size_t prev_block_size=0;
        for(int idx=0;idx<(len_ls+num_alloc_blocks);idx++){
		ptr_raw = (prev_block_ptr)+(prev_block_size+header_size);
		ptr_node = (struct node*)ptr_raw;
		printf("block index = %d , size = %d , addr = %p , ",idx,ptr_node->size,ptr_raw);
		if((int)(ptr_node->next)==magic_num){
			printf("status = Allocated\n");
		}else{
			printf("status = FREE\n");
	        }
		prev_block_ptr = ptr_raw;
		prev_block_size = ptr_node->size;
        }
}
