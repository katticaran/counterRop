
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <errno.h>
#include "../hashtable/hashtable.h"
#include "../list/list.h"
#include <string.h>
#include <stdlib.h>
#include <udis86.h>
//#include <bits/sigaction.h>

extern hashtable_t* dictionary;
extern void printDis(intptr_t buffer);
extern ud_t ud_obj;
extern list_t* address_list;
extern list_t* valid_address_list;

#define PAGE_SIZE 4096
uint8_t start_byte;

void trap_handler(int signal, siginfo_t* info, void* cont); 

int setup_analyzer() {
  struct sigaction sig_action;
  memset(&sig_action, 0, sizeof(sig_action));
  sig_action.sa_sigaction = trap_handler;
  sigemptyset(&sig_action.sa_mask);
  sig_action.sa_flags = SA_SIGINFO;
  sigaction(SIGTRAP, &sig_action, 0);
  return 0;
}



uint8_t trapSetup(intptr_t address) {
  printf("\nEnabling modifications to the address at %lx \n", address);
  intptr_t page_start = address & ~(PAGE_SIZE-1) ;

  printf("Making the start of the page readable, writable, and executable\n");
  //making the page writable, readable and executable
  if (mprotect((void*) page_start, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
    printf("mprotect failed: %s\n",strerror(errno));
    exit(2); 
  }

  printf("Setting the first byte to 0xCC\n");
  //setting the first byte to 0xCC causes a SIGTRAP signal for single-stepping
  uint8_t start_byte = ((uint8_t*)address)[0];
  printf("The original starting byte: %x\n\n",start_byte);
  ((uint8_t*)address)[0] = 0xCC;


return start_byte;
}



void trap_handler(int signal, siginfo_t* info, void* cont) {
  printf("Trap handler triggered\n");
  if (signal != SIGTRAP) {
    printf("Signal received was not a SIGTRAP \n");
    exit(2);
  }

  ucontext_t* context = reinterpret_cast<ucontext_t*>(cont);
  intptr_t address= context->uc_mcontext.gregs[REG_RIP] - 1;
  printf("Instruction at %lx\n",address);
  intptr_t page_start = address & ~(PAGE_SIZE-1) ;

  printf("Making the start of the page readable, writable, and executable\n");
  //making the page writable, readable and executable
  if (mprotect((void*) page_start, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
    printf("mprotect failed: %s\n",strerror(errno));
    exit(2); 
  }
  uint8_t start_byte = ((uint8_t*)address)[0];
  printf("The original starting byte: %x\n\n",start_byte);
 ((uint8_t*)address)[0] =  hashtable_find(dictionary, &address);
 /* printf("The REAL startbyte = %x\n", value); */
 /* ((uint8_t*)address)[0] = value; */


 
  intptr_t buffer;
  char** readAdd;
  char* realAdd;
  intptr_t indirectAddress, callAddress, validAddress, jumpAddress;
  const ud_operand_t*  opr;


  ud_set_input_buffer(&ud_obj, (unsigned char*)address,  100);
  ud_disassemble(&ud_obj);
  unsigned int size =  ud_insn_len(&ud_obj);
  const uint8_t* base = ud_insn_ptr(&ud_obj);
  opr = ud_insn_opr(&ud_obj, 0);
  switch (opr->type){
  case UD_OP_MEM:
    {
    printf("\t\t\t\tIt is a MEM\n");
    intptr_t offset = opr->lval.sdword;
    indirectAddress = (intptr_t)(base+size+offset);
    printf("\t\t\t\tIndirect Address = 0x%lx\n", indirectAddress);
  
    readAdd = (char**)(indirectAddress);
    realAdd = *readAdd;
    callAddress = (intptr_t)realAdd;
  
    printf("\t\t\t\tBase:0x%lx   Size: %d   Offset:0x%lx  \n", (unsigned long)base, size, offset);
    printf("\t\t\t\tMemory Address = 0x%lx\n", (signed long)(indirectAddress));
    printf("\t\t\t\tExtracted Address = 0x%lx\n", (signed long)(callAddress));
    list_insert(address_list, callAddress);
    break;
    }
  case UD_OP_REG:
    printf("\t\t\t\tIt is a REG \n");
    break;
  default:
    printf("\t\t\t\tError: No Idea\n");
    exit(2);
    break;
  }

  // validAddress = (intptr_t)(base+size);
  //list_insert(valid_address_list, validAddress);


  while ((buffer= list_pop(address_list)) != (intptr_t)NULL){
    printDis(buffer);
  }
  

  
  
  printf("Finished trap handler\n");

}


