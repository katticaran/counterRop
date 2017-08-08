#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "boundTree.h"

#define SLOTS 100
int a[SLOTS];
int b[SLOTS];
int c[SLOTS];


int jumpFlag;

int boundVal;
intptr_t lowerVal, upperVal;
boundTreeData_t* data;
 
int executeInstructionAt(int i);

int main(){

  time_t t;
  int i;

  srand((unsigned) time(&t));
  for( i = 0 ; i < SLOTS ; i++ ) 
    {
      int randVal =  rand();
      a[i] = randVal % 10;
      b[i] = 0;
      randVal =  rand();
      c[i] = randVal % SLOTS;
    }

  for( i = 0 ; i < SLOTS ; i++ ) 
    {
      printf("a[%d] :%d , c[%d] :%d \n", i, a[i], i, c[i]);
    }
    
  boundTree_t* thisboundTree = boundTree_new();
  boundTreeData_t* data = boundFind(thisboundTree,0);
  boundVal = data->bound;
  lowerVal = 0;

  if (data != NULL){
    int retVal = executeInstructionAt(0);
    while (retVal != -1){
      if (jumpFlag == 1)
        {
          jumpFlag = 0;
          direct_boundTree_insert(data, lowerVal , upperVal);
          lowerVal = retVal;
          data = boundFind(thisboundTree,lowerVal);
          if (data == NULL){
                 
            break;
          }
        }
      
      retVal = executeInstructionAt(retVal);
    }
       
  }
  boundTree_delete(thisboundTree);
  return 0;

}


int executeInstructionAt(int i){
  if (i > boundVal || i > SLOTS-1)
    return -1;
      
  if (b[i] == 0)
    b[i] = 1;
  else
    b[i] = 999;


  if (a[i] <= 2){
    jumpFlag = 1;
    upperVal = i;
    return (c[i]);
  }

  //return
  else if (a[i] == 9){
    upperVal = -1;
    return -1;
  }
      
  return (i+1);
}

