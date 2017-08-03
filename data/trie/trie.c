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
#include "trie.h"

trie_t *trie_new(){
  trie_t* theTrie = (trie_t*)malloc(sizeof(trie_t));
  theTrie->isInitialized = 0;
  return (theTrie);
}

trie_t* trie_insert(trie_t* trie, intptr_t lowerVal, intptr_t upperVal){
  if (trie == NULL)
    return NULL;
  
  if (trie->isInitialized == 0){
    trieNode_t* node = (trieNode_t*)malloc(sizeof(trieNode_t));
    node->lowerBound = lowerVal;
    node->upperBound = upperVal;
    node->leftChild = NULL;
    node->rightChild = NULL;
    trie->isInitialized = 1;
    trie->root = node;
    return trie;
  }
    
  trieNode_t* currentNode = trie->root;
  trieNode_t* prevNode = trie->root;
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

  trieNode_t* newNode = (trieNode_t*)malloc(sizeof(trieNode_t));
  newNode->lowerBound = lowerVal;
  newNode->upperBound = upperVal;
  newNode->leftChild = NULL;
  newNode->rightChild = NULL;

  if (isLeft)
    prevNode->leftChild = newNode;
  else
    prevNode->rightChild = newNode;
    
  return trie;
}

trieData_t* boundFind(trie_t* trie, intptr_t lowerVal){
  if (trie == NULL)
    return NULL;
  
  trieData_t* metaData = (trieData_t*)malloc(sizeof(trieData_t));
  
  if (trie->isInitialized == 0){
    metaData->trie = trie;
    metaData->isLeftChild = 0;
    metaData->bound = lowerVal + 999;
    metaData->node = trie->root;
    return metaData;
  }
  
  trieNode_t* currentNode = trie->root;
  trieNode_t* prevNode = trie->root;
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
    }
    else{
      free (metaData);
      return NULL;
    }
  }


  metaData->trie = trie;
  metaData->isLeftChild = isLeft;
  metaData->bound = bound;
  metaData->node = prevNode;

  return metaData;

}

  
  

trie_t* direct_trie_insert(trieData_t* metaData, intptr_t lowerVal,
                          intptr_t upperVal){


  trieNode_t* newNode = (trieNode_t*)malloc(sizeof(trieNode_t));
  newNode->lowerBound = lowerVal;
  newNode->upperBound = upperVal;
  newNode->leftChild = NULL;
  newNode->rightChild = NULL;
  
  if (metaData->trie->isInitialized == 0)
    {
      metaData->trie->isInitialized = 1;
      metaData->trie->root = newNode;
    }
  else if (metaData->isLeftChild)
  metaData->node->leftChild = newNode;
 else
   metaData->node->rightChild = newNode;

 trie_t* trie = metaData->trie;
 free(metaData);
 return trie;
}



void deleteNode(trieNode_t* node){
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


void trie_delete(trie_t* trie){
  deleteNode(trie->root);
  free(trie);  
}


