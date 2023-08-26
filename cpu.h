#pragma once
#include <fstream>
#include <semaphore.h>
#include <thread>
#include <unistd.h>

#include "constants.h"
#include "memory.h"
#include "keyboard.h"
#include "Beeper.h"

class Cpu{
public:
    Cpu(Memory* memory, Keyboard* keyboard);
    Memory* memory;
    Keyboard* keyboard;
    int display[MACHINE_HEIGHT][MACHINE_WIDTH];
    void draw_sprite(int x, int y, int n);
    void load_rom(char* rom_name);
    void fetch_decode_execute();
    bool screen_modified = false;
    sem_t delay_sem;
    sem_t sound_sem;
    void delay_timer();
    void sound_timer();
};