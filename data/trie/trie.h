/*
list.h by JOEL KATTICARAN.
Date: 26-Jun-2017

Compilation: run Makefile using 'make'

Purpose: A Queue datastructure.
 */


#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>

typedef struct trieNode{
  intptr_t lowerBound;
  intptr_t upperBound;
  struct trieNode* leftChild;
  struct trieNode* rightChild;
}trieNode_t;

typedef struct trie{
  struct trieNode* root;
  int isInitialized;
}trie_t;

typedef struct trieData{
  trie_t* trie;
  int isLeftChild;
  intptr_t bound;
  trieNode_t* node;
}trieData_t;

//creates a new list data Structure.
trie_t *trie_new();

//inserts a range into the trie. If upperVal is greater than the
// allowed upperBound, then the range is lowerVal to upperBound-1.
// Returns NULL if lowerVal is inbetween an already existing range
trie_t* trie_insert(trie_t* trie, intptr_t lowerVal, intptr_t upperVal);

trieData_t* boundFind(trie_t* trie, intptr_t lowerVal);

//deletes the trie and frees all allocated memory.
void trie_delete(trie_t *trie);

trie_t* direct_trie_insert(trieData_t* metaData, intptr_t lowerVal,
                           intptr_t upperVal);
#endif


