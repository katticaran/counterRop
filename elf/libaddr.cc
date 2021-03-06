
#include "elf++.hh"
#include "interpose.hh" //interposing exit functions
#include "../data/hashtable/hashtable.h"
#include "../data/list/list.h"
#include "../data/trap/trap.h"
#include "../data/boundTree/boundTree.h"

#include <udis86.h>
#include <link.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <map>
#include <fstream>
#include <iostream>

#define PAGE_SIZE 4096
#define NUM_SLOTS 500


using namespace std;
uint64_t offset;
uint64_t func_address; //the address of the target function
fstream return_file;
fstream write_file;

//boundTree 
boundTreeData_t* data;
boundTree_t* thisboundTree = boundTree_new();
intptr_t boundVal;
intptr_t lowerVal, upperVal;

int res = setup_analyzer();
list_t* start_byte_list = list_new();
list_t* address_list = list_new();
//list_t* disas_address_list = list_new();
list_t* valid_address_list = list_new();
hashtable_t* original_start_bytes = hashtable_new(NUM_SLOTS);

static int callback(struct dl_phdr_info *info, size_t size, void *data) ;
string file_readline(string path);
uint64_t find_address(const char* file_path, string func_name);
vector<string> find_in_dir(string dir, string substr);
void printDis(intptr_t buffer);
ud_t ud_obj;
void print_proc_maps();

  
typedef int (*main_fn_t)(int, char**, char**);
main_fn_t og_main;

static int wrapped_main(int argc, char** argv, char** env) {
  cerr << "Entered main wrapper\n";
        
  func_address = find_address("/proc/self/exe", "main");
        
  // printf("FUNC ADDRESS: 0x%lx\n", func_address);


  list_insert(address_list, (intptr_t)func_address);

  intptr_t buffer;

  // udis initialization

  ud_init(&ud_obj);
  ud_set_mode(&ud_obj, 64);
  ud_set_syntax(&ud_obj, UD_SYN_INTEL);
  ud_set_vendor(&ud_obj, UD_VENDOR_INTEL);
 
  while ((buffer = list_pop(address_list)) != (intptr_t)NULL){
    data = boundFind(thisboundTree,buffer);
    if (data != NULL){
      lowerVal = buffer;
      boundVal = data->bound;
      printDis(buffer);
    }
  }

  
  printf("\n\nProgram Start\n\n");
  og_main(argc, argv, env);
  printf("Program End\n");
  
  list_delete(address_list);
  // list_delete(disas_address_list);
  //  shutdown();
  return 0; 
        
}




void printDis(intptr_t buffer)
{

  printf("\nFUNC ADDRESS: 0x%lx\n", (intptr_t)buffer);
  ud_set_input_buffer(&ud_obj, (unsigned char*)buffer,  100);

  char** readAdd;
  char* realAdd;

  const ud_operand_t*  opr;
  uint8_t startByte;
  unsigned int size;
  const uint8_t* base;
  const uint8_t* prevBase;
  intptr_t offset;
  intptr_t callAddress, jumpAddress, validAddress;
  int breakFlag = 1; //to break while loop on an undonditional jmp
  int memFlag = 0;
  int regFlag = 0; //to access the jump location from memory
  int condJumpFlag = 1;
  base = (const uint8_t *)buffer;
  printf("BOUNDARY VALUE: 0x%lx\n", boundVal);
  //inspect each instruction
  while (breakFlag && ud_disassemble(&ud_obj)) {
    prevBase = base;
    base = ud_insn_ptr(&ud_obj);
    if ((intptr_t)base > boundVal){
      break;
    }
      
    printf("\t%lx: %s\n", (intptr_t)base, ud_insn_asm(&ud_obj));
       
     
    // get out if I hit a return instruction. 
    if  ((ud_insn_mnemonic(&ud_obj)) ==  UD_Iret)
      //CHANGE FIRST BYTE TO 0XCC
      break;

    //check what kind of control flow instruction it is    
    switch (ud_insn_mnemonic(&ud_obj)){

    case UD_Icall: {
      printf("\t\t\t\tTIS A CALL!\n");
      // getting operand information required to find call address.
      opr = ud_insn_opr(&ud_obj, 0);
      switch (opr->type){
      case UD_OP_MEM:
        memFlag = 1;
        printf("\t\t\t\tIt is a MEM\n");
        break;
      case UD_OP_JIMM:
        printf("\t\t\t\tIt is a JIMM \n");
        break;
      default:
        printf("\t\t\t\tNo Idea\n");
        break;
      }

        
      size =  ud_insn_len(&ud_obj);
      base = ud_insn_ptr(&ud_obj);
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
      callAddress = (intptr_t)(base+size+offset);
      // callAddress = (intptr_t) (offset);
      printf("\t\t\t\tCalled Address = 0x%lx\n", callAddress);   


      if (memFlag == 1){
        memFlag = 0;
        list_insert(start_byte_list, (intptr_t)base);
        printf("\t\t\t\tSETTING THE TRAPBYTE\n");
        startByte = trapSetup((intptr_t)base);
        hashtable_insert(original_start_bytes, (intptr_t*)&base, &startByte);
        break;
      }
      else {              

        list_insert(address_list, callAddress);
        validAddress = (intptr_t)(base+size);
        list_insert(valid_address_list, validAddress);
        break;
      }
    }

    case UD_Ijmp: 
      breakFlag = 0;
    case UD_Ijb:
    case UD_Ijbe:
    case UD_Ijecxz:
    case UD_Ijle:
    case UD_Ijl:
    case UD_Ijae:
    case UD_Ija:
    case UD_Ijge:   
    case UD_Ijno:
    case UD_Ijnp:
    case UD_Ijg:
    case UD_Ijns:
    case UD_Ijnz:
    case UD_Ijo:
    case UD_Ijp:
    case UD_Ijs:
    case UD_Ijz: {
      printf("\t\t\t\tTIS A JUMP!\n");
      opr  = ud_insn_opr(&ud_obj, 0);
      switch (opr->type){
      case UD_OP_MEM:
        memFlag = 1;
        printf("\t\t\t\tIt is a MEM\n");
        break;
      case UD_OP_JIMM:
        printf("\t\t\t\tIt is a JIMM \n");
        break;
      case UD_OP_IMM:
        printf("\t\t\t\tIt is a IMM \n");
        break;
      case UD_OP_PTR:
        printf("\t\t\t\tIt is a PTR \n");
        break;
      case UD_OP_CONST:
        printf("\t\t\t\tIt is a CONST \n");
        break;
      case UD_OP_REG:
        regFlag = 1;
        printf("\t\t\t\tIt is a REG \n");
        break;
      default:
        printf("\t\t\t\tNo Idea\n");
        break;
      }
      
        
      size =  ud_insn_len(&ud_obj);
      base = ud_insn_ptr(&ud_obj);
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


        
      if (memFlag == 1 || regFlag == 1){
        memFlag = 0;
        regFlag = 0;   
        printf("\t\t\t\tSETTING THE TRAPBYTE\n");
        list_insert(start_byte_list, (intptr_t)base);
        startByte = trapSetup((intptr_t)base);
        hashtable_insert(original_start_bytes, (intptr_t*)&base, &startByte);
        break;
          
      } else {
        printf("\t\t\t\tBase:0x%lx   Size: %d   Offset:0x%lx  \n", (unsigned long)base, size, offset); 
        jumpAddress = (intptr_t)(base+size+offset);
        printf("\t\t\t\tJump Address = 0x%lx\n",jumpAddress);
        list_insert(address_list, jumpAddress);
        break;
      }
    }

    default :
      break;
    }    
  } //while-loop
  upperVal = (intptr_t)base;
  printf("\nBOUND-TREE INSERT: 0x%lx - 0x%lx\n", lowerVal, upperVal);
  direct_boundTree_insert(data, lowerVal, upperVal);
 
  printf("\n\n");
}



 
uint64_t find_address(const char* file_path, string func_name) {
  dl_iterate_phdr(callback, NULL);
        
  uint64_t addr = 0;

  int read_fd = open(file_path, O_RDONLY);
  if (read_fd < 0) {
    fprintf(stderr, "%s: %s\n", file_path, strerror(errno));
    exit(2);
  }

  elf::elf f(elf::create_mmap_loader(read_fd));
  for (auto &sec : f.sections()) {
    if (sec.get_hdr().type != elf::sht::symtab &&
        sec.get_hdr().type != elf::sht::dynsym) continue;

    fprintf(stderr, "Section '%s':\n",
            sec.get_name().c_str());
    fprintf(stderr, "%-16s %-6s %-5s %-5s %-5s %-5s\n",
            "Address", "Size", " Binding", "Index",
            "Name", "Type");
                

    for (auto sym : sec.as_symtab()) {
      auto &d = sym.get_data();
      if (d.type() != elf::stt::func ||
          sym.get_name() != func_name) continue;

                        

      fprintf(stdout, "0x%-16lx %-5lx %-7s %-5s %-5s %-5s\n",
              offset + d.value, d.size,
              to_string(d.binding()).c_str(),
              to_string(d.shnxd).c_str(),
              sym.get_name().c_str(),
              to_string(d.type()).c_str());
                        
      addr = offset + d.value; 
    }
  }       
  return addr;       
}

static int callback(struct dl_phdr_info *info, size_t size, void *data) {
  // or info->dlpi_name == "\0" if first run doesn't work ?
  static int run = 0;
  if (run) return 0;

  offset = info->dlpi_addr; 
  run = 1;
  return 0; 
}


string file_readline(string path) {
  ifstream in(path);
  string str;
  in >> str;
  return str;
}

vector<string> find_in_dir(string dir, string substr) {
  vector<string> res;
  DIR* dirp = opendir(dir.c_str());
  struct dirent* dp;
  while((dp = readdir(dirp)) != NULL) {
    string path = string(dp->d_name);
    if(path.find(substr) != string::npos) {
      res.push_back(path);
    }
  }
  closedir(dirp);
  return res;
}

INTERPOSE (exit)(int rc) {
  // shutdown();
  real::exit(rc);
}

INTERPOSE (_exit)(int rc) {
  // shutdown();
  real::_exit(rc);
}

INTERPOSE (_Exit)(int rc) {
  //  shutdown();
  real::_Exit(rc); 
}


/**
 * Intercepts __libc_start_main call to override main of the program calling 
 the analyzer tool

 * Code retrieved from 
 https://github.com/plasma-umass/coz/blob/master/libcoz/libcoz.cpp and
 https://github.com/ccurtsinger/interpose

*/
extern "C" int __libc_start_main(main_fn_t main_fn, int argc, char** argv,
                                 void (*init)(), void (*fini)(),
                                 void (*rtld_fini)(), void* stack_end) {


  printf("INPUT ARGS: %s, %s, %s\n", argv[0], argv[1], argv[2]);
  print_proc_maps();  
  //Find original __libc_start_main
  auto og_libc_start_main =
    (decltype(__libc_start_main)*)dlsym(RTLD_NEXT, "__libc_start_main");
   
  //Save original main function
  og_main = main_fn;

  //Running original __libc_start_main with wrapped main
  return og_libc_start_main(wrapped_main, argc, argv, init,
                            fini, rtld_fini, stack_end);

    
}




void print_proc_maps()
{
  int pid = getpid();
  char maps_file_name[1024];
  sprintf(maps_file_name, "/proc/%d/maps", pid);
   
  FILE *input = fopen(maps_file_name, "r");
  FILE *output = fopen("maps.log", "w");

  while(!feof(input))
    {
      char *line = 0;
      size_t n = 0;
      getline(&line, &n, input);
      fprintf(output, "%s", line);
    }
   
  fclose(input);
  fclose(output);
}



