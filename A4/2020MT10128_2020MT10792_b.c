#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>

FILE* fp;
pthread_t* threads_arr;
int threads_arr_size;
int threads_arr_cap;

//GLOBALS
struct avl_node
{
    int data;
    struct avl_node *left;
    struct avl_node *right;
    struct avl_node *parent;
    int height;
    pthread_mutex_t* node_lk;
};
typedef struct avl_node avl_node;


struct args_{
	avl_node* node;
	int val;
} typedef thread_args;



void init_glob_vars(){
	threads_arr_size = 0;
	threads_arr_cap = 200;
	threads_arr = malloc(threads_arr_cap*sizeof(pthread_t));
}

avl_node *newNode(int x)
{
    avl_node *node = malloc(sizeof(avl_node)*1);
    node->data = x;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->height = 1;
    return node;
}

avl_node* root;
int num_nodes;
int max(int a, int b){ return (a > b ? a : b);}

//HELPERS
int getHeight(avl_node* node)
{
    if(node == NULL) return 0;
    return node->height;
}
int getBalance(avl_node* node)
{
    return getHeight(node->left) - getHeight(node->right);
}
bool checkBalance(avl_node* node)
{
    int x = getBalance(node);
    if(x == 0 || x == 1 || x == -1) return true;
    return false;
}
void updateHeight(avl_node* node)
{
    node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
}

//ROTATIONS
avl_node* SingleLeftRotation(avl_node* node)
{
    avl_node* y = node->right;
    avl_node* T2 = y->left;
    
    //update left, right
    y->left = node;
    node->right = T2;
    
    //update height
    updateHeight(node);
    updateHeight(y);
    return y;
}

avl_node* SingleRightRotation(avl_node* node)
{
    avl_node* y = node->left;
    avl_node* T3 = y->right;
    
    //update left, right
    y->right = node;
    node->left = T3;
    
    //update height
    updateHeight(node);
    updateHeight(y);
    return y;
}

avl_node* DoubleLeftRightRotation(avl_node* node)
{
    avl_node* x = SingleLeftRotation(node->left);
    node->left = x;
    x = SingleRightRotation(node);
    return x;
}

avl_node* DoubleRightLeftRotation(avl_node* node)
{
    avl_node* x = SingleRightRotation(node->right);
    node->right = x;
    x = SingleLeftRotation(node);
    return x;
}

avl_node* balanceNode(avl_node* node, int x)
{
    int balance = getBalance(node);
    //printf("%d %d %d\n", x, balance, getBalance(node->left));
    if(balance > 1 && getBalance(node->left) >= 0)
        return SingleRightRotation(node);
    if(balance < -1 && getBalance(node->right) <= 0)
        return SingleLeftRotation(node);
    if(balance > 1 && getBalance(node->left) < 0)
        return DoubleLeftRightRotation(node);
    if (balance < -1 && getBalance(node->right) > 0)
        return DoubleRightLeftRotation(node);
    return node;
}

avl_node* in_successor(avl_node* node)
{
    avl_node* temp = node;
    while (temp->left != NULL) temp = temp->left;
    return temp;
}


//FUNCTIONS



void pre_order_traversal(avl_node* node)
{
    if(node == NULL) return;
    printf("%d ", node->data);
    pre_order_traversal(node->left);
    pre_order_traversal(node->right);
}

void* thread_pre_order(thread_args* args){
	pre_order_traversal(args->node);
}



void in_order_traversal(avl_node* node)
{
    if(node == NULL) return;
    in_order_traversal(node->left);
    printf("%d ", node->data);
    in_order_traversal(node->right);
}

void* thread_in_order(thread_args* args){
        in_order_traversal(args->node);
}


avl_node* insert_node(avl_node* node, int x)
{
    //printf("INSERT_%d\n", x);
    num_nodes += 1;
    if(node == NULL) return newNode(x);
    
    if(x < node->data)
        node->left = insert_node(node->left, x);
    else if(x > node->data)
        node->right = insert_node(node->right, x);
    
    updateHeight(node);
    if(checkBalance(node)) return node;
    return balanceNode(node, x);
}

void* thread_insert_node(thread_args* args){
    root =  insert_node(args->node,args->val);// root is a shared variable 
}


avl_node* delete_node(avl_node* node, int x)
{
    //printf("DELETE %d\n", node->data);
    num_nodes -= 1;
    if(node == NULL) return node;
    
    if(x < node->data)
        node->left = delete_node(node->left, x);
    else if(x > node->data)
        node->right = delete_node(node->right, x);
    else
    {
        if(node->left == NULL && node->right == NULL)
            return node = NULL;
        avl_node* temp;
        if(node->left == NULL || node->right == NULL)
        {
            if(node->left == NULL) temp = node->right;
            else if(node->right == NULL) temp = node->left;
            *node = *temp;
            free(temp);
        }
        else
        {
            temp = in_successor(node->right);
            node->data = temp->data;
            node->right = delete_node(node->right, temp->data);
        }
    }
    updateHeight(node);
    if(checkBalance(node)) return node;
    return balanceNode(node, x);
}


void* thread_delete_node(thread_args* args){
    root =  delete_node(args->node,args->val);// root is a shared variable
}

bool contains_node(avl_node* node, int x)
{
    //printf("CHECK %d\n", x);
    if(node == NULL) return false;
    if(x == node->data) return true;
    if(x < node->data) return contains_node(node->left, x);
    return contains_node(node->right, x);
}

void* thread_contains_node(thread_args* args){
    bool found =  contains_node(args->node,args->val);// root is a shared variable
}


int main(int argc, char* argv[])
{
    root = NULL;
    num_nodes = 0;
    fp = fopen(argv[1], "r");
    if(NULL == fp){
        printf("File can't be opened\n");
        return 1;
    }
    // reading line by line, max 256 bytes
    thread_args* t_args;
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];
    while (fgets(buffer, MAX_LENGTH, fp)){
        //tokenise buffer
	threads_arr_size++;
	if(threads_arr_size==threads_arr_cap){
		threads_arr_cap*=2;
		threads_arr = realloc(threads_arr,threads_arr_cap*sizeof(pthread_t));
	}
        char* ptr;
        char *word = strtok(strdup(buffer)," ");
	t_args = malloc(sizeof(thread_args));
        if(strcmp("insert", word) == 0)
        {
            word = strtok(NULL,"\n");
            int x = strtod(strdup(word), &ptr);
	    t_args->val = x;
	    t_args->node = root;
	    // pthread_create(&(threads_arr[threads_arr_size-1]),NULL,thread_insert_node,t_args));
            root = insert_node(root, x);
        }
        else if(strcmp("delete", word) == 0)
        {
            word = strtok(NULL,"\n");
            int x = strtod(strdup(word), &ptr);
	    t_args->val = x;
	    t_args->node = root;
	     // pthread_create(&(threads_arr[threads_arr_size-1]),NULL,thread_delete_node,t_args));
            root = delete_node(root, x);
        }
        else if(strcmp("contains", word) == 0)
        {
            word = strtok(NULL,"\n");
            int x = strtod(strdup(word), &ptr);
            t_args->val = x;
            t_args->node = root;
	     // pthread_create(&(threads_arr[threads_arr_size-1]),NULL,thread_contains_node,t_args));
            if(contains_node(root, x)) printf("YES\n");
            else printf("NO\n");
        }
        else if(strcmp("in", word) == 0)
        {
            word = strtok(NULL,"\n");
            t_args->node = root;
	     // pthread_create(&(threads_arr[threads_arr_size-1]),NULL,thread_in_order,t_args));
            in_order_traversal(root);
            printf("\n");
        }
        else if(strcmp("pre", word) == 0)
        {
            word = strtok(NULL,"\n");
	    t_args->node = root;
	     // pthread_create(&(threads_arr[threads_arr_size-1]),NULL,thread_pre_order,t_args));
            pre_order_traversal(root);
            printf("\n");
        }
    }
    fclose(fp);
    
    /*
    FILE* fp = fopen(argv[2], "w");
    fclose(fp);
    */
    
    return 0;
}
