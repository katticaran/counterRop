/*
list.h by JOEL KATTICARAN.
Date: 26-Jun-2017

Compilation: run Makefile using 'make'

Purpose: A Queue datastructure.
 */


#ifndef LIST_H
#define LIST_H

#include <stdint.h>

typedef struct listNode{
  intptr_t address;
  struct listNode* next;
  struct listNode* prev;
}listNode_t;

typedef struct list{
  struct listNode* head;
  struct listNode* tail;
}list_t;

//creates a new list data Structure.
list_t *list_new();


intptr_t list_pop(list_t* list);

//finds an element within the list and returns the data if found.
//returns 0 if not found.
int list_find(list_t *list, intptr_t address);

//Inserts a new element into the list by creating a node, and copying
int list_insert(list_t *list, intptr_t address);

//deletes the list and frees all allocated memory.
int list_delete(list_t *list);

#endif


