#include "memory.h"

Memory::Memory()
{
    unsigned char char_sprites[] =
        {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        };

    for(int i=0; i<16*5; i++) main_memory[i] = char_sprites[i];
    for(int i=16*5; i<MACHINE_MEMORY_SIZE; i++) main_memory[i] = 0;
    for(int i=0; i<NUM_V_REGS; i++) Vx[i] = 0;
    for(int i=0; i<STACK_SIZE; i++) stack[i] = 0;
    sp = 0;
    I = 0;
}

unsigned char Memory::read_byte(int address)
{
    if (address < 0 || address >= MACHINE_MEMORY_SIZE)
    {
        printf("Error: address out of bounds.");
        exit(-1);
    }

    return main_memory[address];
}

void Memory::write_byte(int address, unsigned char value)
{
    if (address < 0 || address >= MACHINE_MEMORY_SIZE)
    {
        printf("Error: address out of bounds.");
        exit(-1);
    }

    main_memory[address] = value;
}

void Memory::push(unsigned short value)
{
    if (sp >= STACK_SIZE)
    {
        printf("Error: Stack overflow.");
        exit(-1);
    }

    stack[sp++] = value; // stack element size is 16
}

unsigned short Memory::pop()
{
    if (sp < 1)
    {
        printf("Error: Stack underflow.");
        exit(-1);
    }
    return stack[--sp];
}

unsigned short Memory::read_short(int address){
    return (main_memory[address] << 8) | main_memory[address+1];
}




