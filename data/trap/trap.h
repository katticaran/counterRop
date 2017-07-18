#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>
#include <inttypes.h>
#include <signal.h>


uint8_t single_step(intptr_t address);
void setup_analyzer();
void trap_handler(int signal, siginfo_t* info, void* cont);


#endif