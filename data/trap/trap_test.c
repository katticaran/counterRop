#include "./trap.h"
#include <stdio.h>
#include <stdlib.h>

int a = 100;

int main(){
  intptr_t val = (intptr_t)&a;
  printf("The address is 0x%lx\n", val);
  printf("The starting byte is 0x%x\n",trapSetup(val)); 
  return a;
}
