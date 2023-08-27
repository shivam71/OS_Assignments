#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[]){
    
    if(argc != 2){
        printf("Invalid Input\n");
    }
    FILE* ptr;
    ptr = fopen(argv[1], "r");
    if(NULL == ptr){
        printf("File can't be opened\n");
    }
    char ch = fgetc(ptr);
    while(ch != EOF){
        printf("%c", ch);
        ch = fgetc(ptr);
    }
    fclose(ptr);
    
    return 0;
}

