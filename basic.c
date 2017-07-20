#include <stdio.h>

int testFunc0(){
  // printf("In testFunc\n");
  return 3;
}

int testFunc1(){
  // char c = getchar();
   putchar('\n');
  return 4;
}

int main(){
int a = 10;
int b = 12;
//printf("The sum is %d\n", a+b);

a = testFunc1();
b = testFunc0();
 
 
return a+b ;
}

