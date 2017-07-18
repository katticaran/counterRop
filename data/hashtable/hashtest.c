#include <stdio.h>
#include "hashtable.h"

int main(){
  intptr_t val9;
  intptr_t val1 = (intptr_t)&val9;
  intptr_t val2 = (intptr_t)&val1;
  uint8_t data1 = 79;
  hashtable_t* ht = hashtable_new(10000);
   hashtable_insert(ht, &val1, &data1);
  
   hashtable_insert(ht, (intptr_t*)&val1, (uint8_t*)&data1);
   hashtable_insert(ht, (intptr_t*)&val2, (uint8_t*)&data1);

   uint8_t* ans = hashtable_find(ht, (intptr_t*)val2);
  printf("Answer: %s\n",ans);
  hashtable_delete(ht);
  return 0;
}
