#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<semaphore.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>

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
FILE* fp;
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

//ROTATIONS
avl_node* SingleLeftRotation(avl_node* node)
{
    /*
      z                                y
     /  \                            /   \
    T1   y     Left Rotate(z)       z      x
        /  \   - - - - - - - ->    / \    / \
       T2   x                     T1  T2 T3  T4
           / \
         T3  T4
     */
    avl_node* y = node->right;
    avl_node* T2 = y->left;
    
    //update left, right
    y->left = node;
    node->right = T2;
    
    //update parent
    avl_node* par = node->parent;
    node->parent = y;
    y->parent = par;
    T2->parent = node;
    
    //update height
    node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
    y->height = max(node->height, getHeight(y->right)) + 1;
    return y;
}

avl_node* SingleRightRotation(avl_node* node)
{
    /*
             z                                      y
            / \                                   /   \
           y   T4      Right Rotate (z)          x      z
          / \          - - - - - - - - ->      /  \    /  \
         x   T3                               T1  T2  T3  T4
        / \
      T1   T2
     */
    avl_node* y = node->left;
    avl_node* T3 = y->right;
    
    //update left, right
    y->right = node;
    node->left = T3;
    
    //update parent
    avl_node* par = node->parent;
    node->parent = y;
    y->parent = par;
    T3->parent = node;
    
    //update height
    node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
    y->height = max(node->height, getHeight(y->left)) + 1;
    return y;
}

avl_node* DoubleLeftRightRotation(avl_node* node)
{
        /*
         z                               z                           x
        / \                            /   \                        /  \
       y   T4  Left Rotate (y)        x    T4  Right Rotate(z)    y      z
      / \      - - - - - - - - ->    /  \      - - - - - - - ->  / \    / \
    T1   x                          y    T3                    T1  T2 T3  T4
        / \                        / \
      T2   T3                    T1   T2
         */
    avl_node* x = SingleLeftRotation(node->left);
    node->left = x;
    x = SingleRightRotation(node);
    return x;
}

avl_node* DoubleRightLeftRotation(avl_node* node)
{
    /*
         z                            z                            x
        / \                          / \                          /  \
      T1   y   Right Rotate (y)    T1   x      Left Rotate(z)   z      y
          / \  - - - - - - - - ->     /  \   - - - - - - - ->  / \    / \
         x   T4                      T2   y                  T1  T2  T3  T4
        / \                              /  \
      T2   T3                           T3   T4
     */
    avl_node* x = SingleRightRotation(node->right);
    node->right = x;
    x = SingleLeftRotation(node);
    return x;
}


//FUNCTIONS
void pre_order_traversal(avl_node* node)
{
    if(node == NULL) return;
    printf("%d ", node->data);
    pre_order_traversal(node->left);
    pre_order_traversal(node->right);
}

void in_order_traversal(avl_node* node)
{
    if(node == NULL) return;
    in_order_traversal(node->left);
    printf("%d ", node->data);
    in_order_traversal(node->right);
}

avl_node* insert_node(avl_node* node, int x)
{
    printf("INSERT %d\n", x);
    num_nodes += 1;
    if(node == NULL) return newNode(x);
    
    if(x < node->data) node->left = insert_node(node->left, x);
    else if(x > node->data) node->right = insert_node(node->right, x);
    
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    
    if(checkBalance(node)) return node;
    
    int balance = getBalance(node);
    if(balance > 1 && x < node->left->data)
    {
        return SingleRightRotation(node);
    }
    if(balance < -1 && x > node->right->data)
    {
        return SingleLeftRotation(node);
    }
    if(balance > 1 && x > node->left->data)
    {
        return DoubleLeftRightRotation(node);
    }
    if (balance < -1 && x < node->right->data)
    {
        return DoubleRightLeftRotation(node);
    }
    return node;
}

avl_node* delete_node(avl_node* node, int x)
{
    printf("DELETE %d\n", x);
    return node;
}

bool contains_node(avl_node* node, int x)
{
    printf("CHECK %d\n", x);
    if(node == NULL) return false;
    if(x == node->data) return true;
    if(x < node->data) return contains_node(node->left, x);
    return contains_node(node->right, x);
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
    const unsigned MAX_LENGTH = 256;
    char buffer[MAX_LENGTH];
    while (fgets(buffer, MAX_LENGTH, fp)){
        //tokenise buffer
        char* ptr;
        char *word = strtok(strdup(buffer)," ");
        if(strcmp("insert", word) == 0)
        {
            word = strtok(NULL,"\n");
            int x = strtod(strdup(word), &ptr);
            root = insert_node(root, x);
        }
        else if(strcmp("delete", word) == 0)
        {
            word = strtok(NULL,"\n");
            int x = strtod(strdup(word), &ptr);
            delete_node(root, x);
        }
        else if(strcmp("contains", word) == 0)
        {
            word = strtok(NULL,"\n");
            int x = strtod(strdup(word), &ptr);
            
            if(contains_node(root, x)) printf("YES\n");
            else printf("NO\n");
        }
        else if(strcmp("in", word) == 0)
        {
            word = strtok(NULL,"\n");
            in_order_traversal(root);
        }
        else if(strcmp("pre", word) == 0)
        {
            word = strtok(NULL,"\n");
            pre_order_traversal(root);
        }
    }
    fclose(fp);
    
    /*
    FILE* fp = fopen(argv[2], "w");
    fclose(fp);
    */
    
    return 0;
}
