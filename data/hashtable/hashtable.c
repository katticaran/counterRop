
#include <errno.h>
#include <stdbool.h>
#include "hashtable.h"
#include "jhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>


hashtable_t *hashtable_new(const int num_slots){
  int i;
  hashEntry_t** hasharray;
  
  hasharray = (hashEntry_t**)malloc(sizeof(hashEntry_t*) * num_slots);
  if (hasharray == NULL){
    perror("Error");
    exit(1);
  }
    
  for (i=0; i<num_slots; i++){
    hasharray[i] = NULL;
  }

  hashtable_t* hashtable = (hashtable_t*)malloc(sizeof(hashtable_t));
  if (hashtable == NULL){
    perror("Error");
    exit(1);
  }
  hashtable->tableAddr = hasharray;
  hashtable->size = num_slots;
  return hashtable;
}



uint8_t* hashtable_find(hashtable_t *ht, intptr_t *key){
  unsigned long value = JenkinsHash((char*)key, ht->size);
  if ( ht->tableAddr[value] == NULL){
    return NULL;
  }
  hashEntry_t* current = ht->tableAddr[value];
  while(current != NULL){
    if (current->key == *key){
      return (&(current->data));
    }
    current = current->next;
  }
  return NULL;
}



bool hashtable_insert(hashtable_t *ht, intptr_t* key, uint8_t* data){
  unsigned long value = JenkinsHash((char*)key, ht->size);
  if ( ht->tableAddr[value] != NULL){
    hashEntry_t* current = ht->tableAddr[value];
    while(current != NULL){
      if (current->key == *key){
	return false;
      }
      current = current->next;
    }
  }
  hashEntry_t* entry = malloc(sizeof(hashEntry_t));
  if (entry == NULL){
    perror("Error");
    exit(1);
  }
  entry->data = *data;
  entry->key = *key;
  // strcpy(entry->key, key);
  entry->next =  ht->tableAddr[value];
  ht->tableAddr[value] = entry;
  return true;
}
 

bool hashtable_delete(hashtable_t *ht){
  if (ht == NULL){
    return false;
  }
  int i;
  for (i=0;i<ht->size; i++){
    hashEntry_t* current = ht->tableAddr[i];
    if (current == NULL)
      continue;
    else{
      hashEntry_t* next = current->next;
      while(next != NULL){
	free(current);
	current = next;
	next = current->next;
      }
      free(current);
    }
  }
  free(ht->tableAddr);
  free(ht);
  return true;
}
