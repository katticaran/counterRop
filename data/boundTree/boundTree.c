/*
 trie by JOEL KATTICARAN.
  Date: 26-June-2017

  Compilation: run Makefile using 'make'

  Return Values
  0: Failure
  !0: Success

*/


//things to do
//1) check if wrapperDataStruct is needed

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits>
#include "boundTree.h"

#define NO_BOUND 999

boundTree_t *boundTree_new(){
  boundTree_t* theboundTree = (boundTree_t*)malloc(sizeof(boundTree_t));
  theboundTree->isInitialized = 0;
  return (theboundTree);
}

boundTree_t* boundTree_insert(boundTree_t* boundTree, intptr_t lowerVal, intptr_t upperVal){
  if (boundTree == NULL)
    return NULL;
  
  if (boundTree->isInitialized == 0){
    boundTreeNode_t* node = (boundTreeNode_t*)malloc(sizeof(boundTreeNode_t));
    node->lowerBound = lowerVal;
    node->upperBound = upperVal;
    node->leftChild = NULL;
    node->rightChild = NULL;
    boundTree->isInitialized = 1;
    boundTree->root = node;
    return boundTree;
  }
    
  boundTreeNode_t* currentNode = boundTree->root;
  boundTreeNode_t* prevNode = boundTree->root;
  int isLeft = 0;
  while(currentNode != NULL){
    if (lowerVal < currentNode->lowerBound){
      isLeft = 1;
      prevNode = currentNode;
      currentNode = currentNode->leftChild;
    }
    else if (lowerVal > currentNode->upperBound){
      isLeft = 0;
      prevNode = currentNode;
      currentNode = currentNode->rightChild;
    }
    else return NULL;
  }

  boundTreeNode_t* newNode = (boundTreeNode_t*)malloc(sizeof(boundTreeNode_t));
  newNode->lowerBound = lowerVal;
  newNode->upperBound = upperVal;
  newNode->leftChild = NULL;
  newNode->rightChild = NULL;

  if (isLeft)
    prevNode->leftChild = newNode;
  else
    prevNode->rightChild = newNode;
    
  return boundTree;
}

boundTreeData_t* boundFind(boundTree_t* boundTree, intptr_t lowerVal){
  if (boundTree == NULL)
    return NULL;
  
  boundTreeData_t* metaData = (boundTreeData_t*)malloc(sizeof(boundTreeData_t));
  
  if (boundTree->isInitialized == 0){
    metaData->boundTree = boundTree;
    metaData->isLeftChild = 0;
    metaData->bound = lowerVal + NO_BOUND;
    metaData->node = boundTree->root;
    return metaData;
  }
  
  boundTreeNode_t* currentNode = boundTree->root;
  boundTreeNode_t* prevNode = boundTree->root;
  int isLeft = 0;
  intptr_t bound = 0;
  while(currentNode != NULL){
      if (lowerVal < currentNode->lowerBound){
      isLeft = 1;
      bound = (currentNode->lowerBound)-1;
      prevNode = currentNode;
      currentNode = currentNode->leftChild;
    }
    else if (lowerVal > currentNode->upperBound){
      isLeft = 0;
      prevNode = currentNode;
      currentNode = currentNode->rightChild;
      bound = lowerVal + NO_BOUND;
    }
    else{
      free (metaData);
      return NULL;
    }
  }


  metaData->boundTree = boundTree;
  metaData->isLeftChild = isLeft;
  metaData->bound = bound;
  metaData->node = prevNode;

  return metaData;

}

  
  

boundTree_t* direct_boundTree_insert(boundTreeData_t* metaData, intptr_t lowerVal,
                          intptr_t upperVal){


  boundTreeNode_t* newNode = (boundTreeNode_t*)malloc(sizeof(boundTreeNode_t));
  newNode->lowerBound = lowerVal;
  newNode->upperBound = upperVal;
  newNode->leftChild = NULL;
  newNode->rightChild = NULL;
  
  if (metaData->boundTree->isInitialized == 0)
    {
      metaData->boundTree->isInitialized = 1;
      metaData->boundTree->root = newNode;
    }
  else if (metaData->isLeftChild)
  metaData->node->leftChild = newNode;
 else
   metaData->node->rightChild = newNode;

 boundTree_t* boundTree = metaData->boundTree;
 free(metaData);
 return boundTree;
}



void deleteNode(boundTreeNode_t* node){
  if (node != NULL){
    deleteNode(node->leftChild);
    deleteNode(node->rightChild);
    free(node);

  }
}


/* void free_tree(Node * node){ */
/*    //post-order like FatalError hinted at */
/*        if (node != NULL) { */
/*         free_tree(node->right); */
/*         free(node->data); //if data was heap allocated, need to free it */
/*         free_tree(node->left); */
/*         free(node); */
/*      }} */


void boundTree_delete(boundTree_t* boundTree){
  deleteNode(boundTree->root);
  free(boundTree);  
}


