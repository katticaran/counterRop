
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <bits/sigaction.h>




#define PAGE_SIZE 4096
uint8_t start_byte;

void trap_handler(int signal, siginfo_t* info, void* cont); 

void setup_analyzer() {
  struct sigaction sig_action;
  memset(&sig_action, 0, sizeof(sig_action));
  sig_action.sa_sigaction = trap_handler;
  sigemptyset(&sig_action.sa_mask);
  sig_action.sa_flags = SA_SIGINFO;
  sigaction(SIGTRAP, &sig_action, 0);

 // start_byte = trapSetup((intptr_t*)func_address);
}



uint8_t trapSetup(intptr_t address) {
  printf("Enabling single step for the function at %lx \n", address);
  intptr_t page_start = address & ~(PAGE_SIZE-1) ;

  printf("Making the start of the page readable, writable, and executable");
  //making the page writable, readable and executable
    if (mprotect((void*) page_start, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
    printf("mprotect failed: %s\n",strerror(errno));
    exit(2); 
  }

  printf("Setting the first byte to 0xCC\n");
  //setting the first byte to 0xCC causes a SIGTRAP signal for single-stepping
  uint8_t start_byte = ((uint8_t*)address)[0];
  printf("The original starting byte: %x\n",start_byte);
  ((uint8_t*)address)[0] = 0xCC;
  printf("Finished enabling single step\n");
  return start_byte;
}



void trap_handler(int signal, siginfo_t* info, void* cont) {
  printf("Trap handler triggered\n");
  if (signal != SIGTRAP) {
    printf("Signal received was not a SIGTRAP \n");
    exit(2);
  }

  //used to keep track of the stack manipulation 
 
  static bool ret_reached = false;
  static bool call_reached = false;
  static int call_count = 0;

   intptr_t func_address;
   uint8_t* instr_first_byte = (uint8_t*)func_address;
   intptr_t ret_addr = 0; //ret addr logging 


   
  printf("Finished trap handler\n");

}


