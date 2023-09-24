#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<sys/mman.h>
#include "mmu.h"

int main(){
    /*
     int* arr = (int*)my_malloc(1025*sizeof(int));
     arr[0]=100;
     arr[1024]=200;
     printf("%d\n",arr[0]);
     printf("%d\n",arr[1024]);
     */
    
    char *str = my_malloc(512);
    debug();
    char *str2 = my_malloc(512);
    debug();
    if(str == NULL || str2 == NULL)
    {
        printf("alloc failed\n");
        return 1;
    }

    strcpy(str, "Hello, world!");
    if(strcmp("Hello, world!", str))
      printf("Hello, world! test failed\n");
    
    else
      printf("Hello, world! test passed\n");
    
    my_free(str);
    debug();
    my_free(str2);
    debug();
    printf("Elementary tests passed\n");
    printf("_%c_\n", str[2]);
    
    /*
    Comprehensive tests:
    1. Populate entire thing in blocks of for loop (a's, b's, c's, and d's) equal size.
    2. Dealloc c's, reallocate and replace with x's.
    3.
    */

    /*** test 1 ****/
    
    //Generating ideal strings for comparison
    char stringA[1024], stringB[1024], stringC[1024], stringD[1024], stringX[1024];
    for(int i = 0; i < 1023; i++)
    {
        stringA[i] = 'a';
        stringB[i] = 'b';
        stringC[i] = 'c';
        stringD[i] = 'd';
        stringX[i] = 'x';
    }

    stringA[1023] = stringB[1023] = stringC[1023] = stringD[1023] = stringX[1023] = '\0';

    char *strA = my_malloc(1024);
    debug();
    char *strB = my_malloc(1024);
    debug();
    char *strC = my_malloc(1024);
    debug();
    char *strD = my_malloc(1024);
    debug();

    for(int i = 0; i < 1023; i++)
    {
        strA[i] = 'a';
        strB[i] = 'b';
        strC[i] = 'c';
        strD[i] = 'd';
    }
    strA[1023] = strB[1023] = strC[1023] = strD[1023] = '\0';

    if(strcmp(stringA, strA) == 0 && strcmp(stringB, strB) == 0 && strcmp(stringC, strC) == 0 && strcmp(stringD, strD) == 0)
      printf("Test 1 passed: allocated 4 chunks of 1KB each\n");
    else
      printf("Test 1 failed: A: %d, B: %d, C: %d, D: %d\n", strcmp(stringA, strA), strcmp(stringB, strB), strcmp(stringC, strC), strcmp(stringD, strD));

    /**** test 2 ****/
    
    my_free(strC);
    debug();
    char *strX = my_malloc(1024);
    debug();
    for(int i = 0; i < 1023; i++)
    {
        strX[i] = 'x';
    }
    strX[1023] = '\0';

    if(strcmp(stringX, strX) == 0)
      printf("Test 2 passed: dealloc and realloc worked\n");
    else
      printf("Test 2 failed: X: %d\n", strcmp(stringX, strX));

    /*** test 3 ***/
    
    char stringY[512], stringZ[512];
    for(int i = 0; i < 511; i++)
    {
        stringY[i] = 'y';
        stringZ[i] = 'z';
    }
    stringY[511] = stringZ[511] = '\0';

    my_free(strB);
    debug();
    char *strY = my_malloc(512);
    debug();
    char *strZ = my_malloc(512);
    debug();
    for(int i = 0; i < 511; i++)
    {
        strY[i] = 'y';
        strZ[i] = 'z';
    }
    strY[511] = strZ[511] = '\0';

    if(strcmp(stringY, strY) == 0 && strcmp(stringZ, strZ) == 0)
      printf("Test 3 passed: dealloc and smaller realloc worked\n");
    else
      printf("Test 3 failed: Y: %d, Z: %d\n", strcmp(stringY, strY), strcmp(stringZ, strZ));


    // merge checks
    //test 4: free 2x512, allocate 1024
    
    my_free(strZ);
    debug();
    my_free(strY);
    debug();
    strY=my_malloc(1024);
    debug();
    for(int i = 0; i < 1023; i++)
    {
        strY[i] = 'x';
    }
    strY[1023] = '\0';

    if(strcmp(stringX, strY) == 0)
      printf("Test 4 passed: merge worked\n");
    else
      printf("Test 4 failed: X: %d\n", strcmp(stringX, strX));

    //test 5: free 2x1024, allocate 2048
    
    my_free(strX);
    debug();
    my_free(strY);
    debug();
    strX= my_malloc(2048);
    debug();
    char  stringM[2048];
    for (int i=0;i<2047;i++){
        strX[i]=stringM[i]='z';
    }
    strX[2047]=stringM[2047]='\0';
    if (!strcmp(stringM, strX))
        printf("Test 5 passed: merge alloc 2048 worked\n");
    else
        printf("Test5 failed\n");
    ///////////////////////////
    debug();
    my_free(strX);
    debug();
    my_free(strA);
    debug();
    my_free(strD);
    debug();
    printf("Test6 Calloc\n");
    int* arr = my_calloc(10,4);
    for(int idx=0;idx<10;idx++){
	printf("%d\n",arr[idx]);
	arr[idx]=idx;
    }
    debug();
    printf("Test 7 Realloc\n");
    arr = my_realloc(arr,80);
    for(int idx=0;idx<20;idx++){
        printf("%d\n",arr[idx]);
        arr[idx]=idx;
    }
    debug();
    my_free(arr);
    debug();
    return 0;
}
