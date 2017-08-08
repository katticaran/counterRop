
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <errno.h>
#include "../hashtable/hashtable.h"
#include "../list/list.h"
#include "../boundTree/boundTree.h"
#include <string.h>
#include <stdlib.h>
#include <udis86.h>
//#include <bits/sigaction.h>

extern hashtable_t* original_start_bytes;
extern void printDis(intptr_t buffer);
extern ud_t ud_obj;
extern list_t* address_list;
extern list_t* valid_address_list;
extern list_t* start_byte_list;
extern intptr_t boundVal, lowerVal, upperVal;
extern boundTree_t* thisboundTree;
extern boundTreeData_t* data;
int regFlag = 0;


#define PAGE_SIZE 4096
uint8_t start_byte;

uint64_t get_register(ud_type_t obj, ucontext_t* context);
void trap_handler(int signal, siginfo_t* info, void* cont); 
intptr_t get_address(const ud_operand_t* indirectAddress,  ucontext_t* context);
void swap_start_byte(intptr_t address);

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

  intptr_t buffer;
  char** readAdd;
  char* realAdd;
  intptr_t indirectAddress, callAddress;
  const ud_operand_t*  opr;

  
  printf("Trap handler triggered\n");
  if (signal != SIGTRAP) {
    printf("Signal received was not a SIGTRAP \n");
    exit(2);
  }

  ucontext_t* context = reinterpret_cast<ucontext_t*>(cont);

  //setting RIP back by 1 to account for the trap
  intptr_t address= --context->uc_mcontext.gregs[REG_RIP] ;
  printf("Instruction at %lx\n",address);
  intptr_t page_start = address & ~(PAGE_SIZE-1) ;

  //making the page writable, readable and executable
  printf("Making the start of the page readable, writable, and executable\n");
  if (mprotect((void*) page_start, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
    printf("mprotect failed: %s\n",strerror(errno));
    exit(2); 
  }

  //replacing the original startByte
  uint8_t start_byte = ((uint8_t*)address)[0];
  ((uint8_t*)address)[0] =  hashtable_find(original_start_bytes, &address);


  ud_set_input_buffer(&ud_obj, (unsigned char*)address,  100);
  ud_disassemble(&ud_obj);
  opr = ud_insn_opr(&ud_obj, 0);

 
  indirectAddress = get_address(opr, context);

  if (regFlag != 0){
    regFlag = 0;
    callAddress = indirectAddress;
  } else {
    readAdd = (char**)(indirectAddress);
    realAdd = *readAdd;
    callAddress = (intptr_t)realAdd;
  }
  
  
   
  printf("\t\t\t\tExtracted Address = 0x%lx\n", (signed long)(callAddress));
  list_insert(address_list, callAddress);
 

  while ((buffer= list_pop(address_list)) != (intptr_t)NULL){
    printf("Within handler\n");
    data = boundFind(thisboundTree,buffer);
    if (data != NULL){
      lowerVal = buffer;
      boundVal = data->bound;
      printDis(buffer);
    }
  }
   
  printf("Finished trap handler\n");

}


intptr_t get_address(const ud_operand_t* opr,  ucontext_t* context){
  intptr_t offset, indirectAddress;
  unsigned int size;
  uint8_t scale = 0;
  
  //get the offset Value
  switch (opr->type){
  case UD_OP_MEM:
    {
      printf("\t\t\t\tIt is a MEM\n");
      switch (opr->size){
      case 8:  offset = opr->lval.sbyte;
        break;
      case 16: offset = opr->lval.sword;
        break;
      case 32: offset = opr->lval.sdword;
        break;
      case 64: offset = opr->lval.sqword;
        break;
      }


      //get size of operand
      size =  ud_insn_len(&ud_obj);
     
      size_t base = get_register(opr->base, context);
      if (opr->base == UD_R_RIP)
        base = base + size;

 
      if (opr->index != UD_NONE) {
        if (opr->scale == 0) scale = 1;
        else scale = opr->scale;
      }

      size_t index = get_register(opr->index, context);

  
      indirectAddress = (intptr_t)(base+offset+(scale * index));
      printf("\t\t\t\tBase:0x%lx   Offset: 0x%lx   Scale:%d  Index:%d \n",
             (unsigned long)base, offset, scale, (int)index);
      printf("\t\t\t\tIndirect Address = 0x%lx\n", indirectAddress);
      return indirectAddress;
    }

  case UD_OP_REG:{
    regFlag = 1;
    printf("\t\t\t\tIt is a REG \n");
    indirectAddress = get_register(opr->base, context);
    printf("THE REGISTER VAL IS %lx\n", indirectAddress);
    return indirectAddress;
 
  }

  default:
    printf("\t\t\t\tError: No Idea\n");
    exit(2);
    break;
  }

}



uint64_t get_register(ud_type_t obj, ucontext_t* context) {
  switch(obj) {
  case UD_R_RIP:
    return context->uc_mcontext.gregs[REG_RIP];
  case UD_R_RAX: case UD_R_EAX: case UD_R_AX: case UD_R_AL: case UD_R_AH:
    return context->uc_mcontext.gregs[REG_RAX];
  case UD_R_RCX: 
    return context->uc_mcontext.gregs[REG_RCX];
  case UD_R_RDX: case UD_R_EDX: case UD_R_DX: case UD_R_DL: case UD_R_DH:
    return context->uc_mcontext.gregs[REG_RDX];
  case UD_R_RBX:
    return context->uc_mcontext.gregs[REG_RBX];
  case UD_R_RSP:
    return context->uc_mcontext.gregs[REG_RSP];
  case UD_R_RBP:
    return context->uc_mcontext.gregs[REG_RBP];
  case UD_R_RSI:
    return context->uc_mcontext.gregs[REG_RSI];
  case UD_R_RDI:
    return context->uc_mcontext.gregs[REG_RDI];             
  case UD_R_R8:
    return context->uc_mcontext.gregs[REG_R8];
  case UD_R_R9:
    return context->uc_mcontext.gregs[REG_R9];              
  case UD_R_R10:
    return context->uc_mcontext.gregs[REG_R10];             
  case UD_R_R11:
    return context->uc_mcontext.gregs[REG_R11];
  case UD_R_R12:
    return context->uc_mcontext.gregs[REG_R12];
  case UD_R_R13:
    return context->uc_mcontext.gregs[REG_R13];
  case UD_R_R14:
    return context->uc_mcontext.gregs[REG_R14];
  case UD_R_R15:
    return context->uc_mcontext.gregs[REG_R15];
  case UD_NONE:
    return 0;
  default:
    printf("Unsupported register!\n");
    return -1; 
  }
}


void shutdown(){
  intptr_t address;

  while((address = list_pop(start_byte_list)) != (intptr_t)NULL)
    {
      swap_start_byte(address);
    }
    
}

void swap_start_byte(intptr_t address){

   intptr_t page_start = address & ~(PAGE_SIZE-1) ;
   
  if (mprotect((void*) page_start, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC) == -1) {
    printf("mprotect failed: %s\n",strerror(errno));
    exit(2); 
  }

  //replacing the original startByte
  ((uint8_t*)address)[0] =  hashtable_find(original_start_bytes, &address);
}


				/* It is a MEM */
				/* Base:0x555555554561   Offset: 0x200a62   Scale:0  Index:0  */
				/* Indirect Address = 0x555555754fc9 */
				/* Extracted Address = 0x7ffff743b6 */
