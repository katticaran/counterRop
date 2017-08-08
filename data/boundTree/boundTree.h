/*
list.h by JOEL KATTICARAN.
Date: 26-Jun-2017

Compilation: run Makefile using 'make'

Purpose: A Queue datastructure.
 */


#ifndef BOUNDTREE_H
#define BOUNDTREE_H

#include <stdint.h>

typedef struct boundTreeNode{
  intptr_t lowerBound;
  intptr_t upperBound;
  struct boundTreeNode* leftChild;
  struct boundTreeNode* rightChild;
}boundTreeNode_t;

typedef struct boundTree{
  struct boundTreeNode* root;
  int isInitialized;
}boundTree_t;

typedef struct boundTreeData{
  boundTree_t* boundTree;
  int isLeftChild;
  intptr_t bound;
  boundTreeNode_t* node;
}boundTreeData_t;

//creates a new list data Structure.
boundTree_t* boundTree_new();

//inserts a range into the boundTree. If upperVal is greater than the
// allowed upperBound, then the range is lowerVal to upperBound-1.
// Returns NULL if lowerVal is inbetween an already existing range
boundTree_t* boundTree_insert(boundTree_t* boundTree, intptr_t lowerVal, intptr_t upperVal);

boundTreeData_t* boundFind(boundTree_t* boundTree, intptr_t lowerVal);

//deletes the boundTree and frees all allocated memory.
void boundTree_delete(boundTree_t *boundTree);

boundTree_t* direct_boundTree_insert(boundTreeData_t* metaData, intptr_t lowerVal, intptr_t upperVal);
#endif


