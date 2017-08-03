#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "trie.h"

#define SLOTS 100
int a[SLOTS];
int b[SLOTS];
int c[SLOTS];


int jumpFlag;

int boundVal;
intptr_t lowerVal, upperVal;
trieData_t* data;
 
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
    
  trie_t* thisTrie = trie_new();
  trieData_t* data = boundFind(thisTrie,0);
  boundVal = data->bound;
  lowerVal = 0;

  if (data != NULL){
    int retVal = executeInstructionAt(0);
    while (retVal != -1){
      if (jumpFlag == 1)
        {
          jumpFlag = 0;
          direct_trie_insert(data, lowerVal , upperVal);
          lowerVal = retVal;
          data = boundFind(thisTrie,lowerVal);
          if (data == NULL){
                 
            break;
          }
        }
      
      retVal = executeInstructionAt(retVal);
    }
       
  }
  trie_delete(thisTrie);
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

