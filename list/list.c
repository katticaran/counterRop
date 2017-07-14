/*
  list by JOEL KATTICARAN.
  Date: 26-June-2017

  Compilation: run Makefile using 'make'

  Purpose: A doubly-linked Queue (FIFO) datastructure. Includes functions 
  that help with insertion, searching and popping.

  Return Values
  0: Failure
  !0: Success

*/


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

list_t *list_new(){
  list_t *theList = (list_t*)malloc(sizeof(list_t));
  theList->head = NULL;
  theList->tail = NULL;
  return theList;
}

int list_find(list_t *list, intptr_t address){
  listNode_t* next = list->head;
  while(next != NULL){
    if (next->address == address){
      //  printf("New %lx, Old %lx\n", next->address, address);
      return 1;
    }
    next = next->next;
  }
  return 0;
}


int list_insert(list_t *list, intptr_t address){
  if ( list_find(list, address) == 0 ){
    listNode_t* newNode = (listNode_t*)malloc(sizeof(listNode_t));
    if (newNode == NULL){
      perror("Error ");
      exit(1);
    }
    newNode->address = address;
    newNode->next = NULL;
    newNode->prev = list->tail;
    if ( list->head == NULL ){
      list->head = newNode;
    }
    if ( list->tail != NULL ){
      list->tail->next = newNode;
    }
    list->tail = newNode;
    return 1;
  }
  return 0;
}


intptr_t list_pop(list_t* list){
  if  (list->head == NULL)
    return 0x0;
  
  listNode_t* tempNode = list->head;
  intptr_t temp = list->head->address;

  if (list->head == list->tail){
    list->head = NULL;
    list->tail = NULL;
  }   else {
    list->head = tempNode->next;
    tempNode->next->prev = NULL;
  }
  
  free (tempNode);
  return temp;
}
  
        

int list_delete(list_t *list){
  if ( list == NULL )
    return 0;
  listNode_t *current;
  while( list->tail != NULL ){
    current = list->tail;
    list->tail = current->prev;
    free(current);
  }
  free(list);
  return 1;
}
