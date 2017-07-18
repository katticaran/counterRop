#include <stdio.h>
#include "list.h"

int main(){
  list_t* list = list_new();
  intptr_t key1 = 0xffffffffffff1234;
  intptr_t key2 = 0xffffffffffff5678;
  intptr_t key3 = 0xffffffffffff9876;
  list_insert(list, key1 );
  list_insert(list, key2 );
  list_insert(list, key3 );
  intptr_t val1 = list_pop(list);
  printf("Val1: %lx\n",val1);
  val1 = list_pop(list);
  printf("Val1: %lx\n",val1);
  val1 = list_pop(list);
  printf("Val1: %lx\n",val1);
  list_delete(list);
}
