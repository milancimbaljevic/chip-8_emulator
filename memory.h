#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

class Memory{
public:
    Memory();
    unsigned char main_memory[MACHINE_MEMORY_SIZE];
    unsigned char read_byte(int address);
    unsigned short read_short(int address);
    void write_byte(int address, unsigned char value);
    unsigned char Vx[NUM_V_REGS]; // VF is used as a flag
    unsigned short I;
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short pc;
    unsigned char sp;
    unsigned short stack[STACK_SIZE];
    void push(unsigned short value);
    unsigned short pop();
};