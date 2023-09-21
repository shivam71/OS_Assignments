#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include "my_mmu.h"

int main(){
     int* arr = (int*)my_malloc(1025*sizeof(int));
     arr[0]=100;
     arr[1024]=200;
     printf("%d\n",arr[0]);
     printf("%d\n",arr[1024]);
}
