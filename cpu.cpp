#include "cpu.h"

extern bool emulator_is_running;

Cpu::Cpu(Memory *memory, Keyboard *keyboard)
{
    this->memory = memory;
    this->keyboard = keyboard;

    for (int i = 0; i < MACHINE_HEIGHT; i++)
    {
        for (int j = 0; j < MACHINE_WIDTH; j++)
        {
            display[i][j] = 0;
        }
    }
}

void Cpu::draw_sprite(int x, int y, int n)
{
    x = memory->Vx[x] % MACHINE_WIDTH;
    y = memory->Vx[y] % MACHINE_HEIGHT;

    memory->Vx[0xF] = 0;

    for (int current_byte = 0; current_byte < n; current_byte++)
    {
        unsigned char current_row = memory->main_memory[memory->I + current_byte];

        for (int i = 0; i < 8; i++)
        {
            if (current_row & (0b10000000 >> i))
            {
                if (y + current_byte >= MACHINE_HEIGHT || x + i >= MACHINE_WIDTH)
                    continue;
                if (display[y + current_byte][x + i] == 1)
                    memory->Vx[0xF] = 1;
                display[y + current_byte][x + i] ^= 1;
            }
        }
    }
}

void Cpu::load_rom(char *rom_name)
{
    std::ifstream rom_file(rom_name, std::ios::in | std::ios::binary);

    int i = 0;

    while (!rom_file.eof())
    {
        if (i + PROGRAM_START >= MACHINE_MEMORY_SIZE)
        {
            printf("Error: Out of memory.\n");
            exit(-1);
        }

        unsigned char byte;
        rom_file.read((char *)&byte, sizeof(char));
        memory->main_memory[PROGRAM_START + i] = byte;

        i++;
    }
    rom_file.close();
}

void Cpu::fetch_decode_execute()
{
    unsigned short instruction = memory->read_short(memory->pc);
    unsigned short nnn_addr = (unsigned short)(instruction << 4) >> 4;
    unsigned char n = instruction & 0x000f;
    unsigned char x = (instruction & 0x0f00) >> 8;
    unsigned char y = (instruction & 0x00f0) >> 4;
    unsigned char kk = instruction & 0x00ff;

    memory->pc += 2;

    if (instruction == 0x00E0)
    {
        // CLS - clear the display
        for (int i = 0; i < MACHINE_HEIGHT; i++)
        {
            for (int j = 0; j < MACHINE_WIDTH; j++)
            {
                display[i][j] = 0;
            }
        }
        screen_modified = true;
    }
    else if (instruction == 0x00EE)
    {
        // RET - return from subroutine
        memory->pc = memory->pop();
    }
    else if (instruction >> 12 == 0x0)
    {
        // SYS - jump to a machine code routine at nnn
        memory->pc = nnn_addr;
    }
    else if (instruction >> 12 == 0x1)
    {
        //  JP addr - jump to location nnn
        memory->pc = nnn_addr;
    }
    else if (instruction >> 12 == 0x2)
    {
        // CALL addr
        memory->push(memory->pc);
        memory->pc = nnn_addr;
    }
    else if (instruction >> 12 == 0x3)
    {
        // SE Vx, byte
        if (memory->Vx[x] == kk)
            memory->pc += 2;
    }
    else if (instruction >> 12 == 0x4)
    {
        // SNE Vx, byte
        if (memory->Vx[x] != kk)
            memory->pc += 2;
    }
    else if (instruction >> 12 == 0x5)
    {
        // SE Vx, Vy
        if (memory->Vx[x] == memory->Vx[y])
            memory->pc += 2;
    }
    else if (instruction >> 12 == 0x6)
    {
        // LD Vx, byte
        memory->Vx[x] = kk;
    }
    else if (instruction >> 12 == 0x7)
    // ADD Vx, byte
    {
        memory->Vx[x] += kk;
    }
    else if (instruction >> 12 == 0x8)
    {
        if (n == 0x0)
        {
            // LD Vx, Vy
            memory->Vx[x] = memory->Vx[y];
        }
        else if (n == 0x1)
        {
            // OR Vx, Vy
            memory->Vx[x] |= memory->Vx[y];
            memory->Vx[0xF] = 0;
        }
        else if (n == 0x2)
        {
            // AND Vx, Vy
            memory->Vx[x] &= memory->Vx[y];
            memory->Vx[0xF] = 0;
        }
        else if (n == 0x3)
        {
            // XOR Vx, Vy
            memory->Vx[x] ^= memory->Vx[y];
            memory->Vx[0xF] = 0;
        }
        else if (n == 0x4)
        {
            // ADD Vx, Vy
            unsigned short add = memory->Vx[x] + memory->Vx[y];
            memory->Vx[x] = add;
            if (add > 255)
                memory->Vx[0xF] = 1;
            else
                memory->Vx[0xF] = 0;
        }
        else if (n == 0x5)
        {
            // SUB Vx, Vy
            unsigned char temp = memory->Vx[x];
            memory->Vx[x] -= memory->Vx[y];
            if (temp > memory->Vx[y])
                memory->Vx[0xF] = 1;
            else
                memory->Vx[0xF] = 0;
        }
        else if (n == 0x6)
        {
            // SHR Vx {, Vy}
            // unsigned char temp = memory->Vx[x];
            // memory->Vx[x] >>= 1;
            // memory->Vx[0xF] = temp & 0x1;
            unsigned char temp = memory->Vx[y];
            memory->Vx[x] = temp >> 1;
            memory->Vx[0xF] = temp & 0x1;
        }
        else if (n == 0x7)
        {
            // SUBN Vx, Vy
            unsigned char temp = memory->Vx[x];
            memory->Vx[x] = memory->Vx[y] - memory->Vx[x];
            if (memory->Vx[y] > temp)
                memory->Vx[0xF] = 1;
            else
                memory->Vx[0xF] = 0;
        }
        else if (n == 0xE)
        {
            // SHL Vx {, Vy}
            // unsigned char temp = memory->Vx[x];
            // memory->Vx[x] <<= 1;
            // memory->Vx[0xF] = temp >> 7;
            unsigned char temp = memory->Vx[y];
            memory->Vx[x] = temp << 1;
            memory->Vx[0xF] = temp >> 7;
        }
    }
    else if (instruction >> 12 == 0x9)
    {
        // SNE Vx, Vy
        if (memory->Vx[x] != memory->Vx[y])
            memory->pc += 2;
    }
    else if (instruction >> 12 == 0xA)
    {
        // Annn - LD I, addr
        memory->I = nnn_addr;
    }
    else if (instruction >> 12 == 0xB)
    {
        // Bnnn - JP V0, addr
        memory->pc = nnn_addr + memory->Vx[0];
    }
    else if (instruction >> 12 == 0xC)
    {
        // Cxkk - RND Vx, byte
        srand(clock());
        memory->Vx[x] = (rand() % 255) & kk;
    }
    else if (instruction >> 12 == 0xD)
    {
        // Dxyn - DRW Vx, Vy, nibble
        draw_sprite(x, y, n);
        screen_modified = true;
    }
    else if (instruction >> 12 == 0xE)
    {
        if (kk == 0x9E)
        {
            // SKP Vx
            if (keyboard->keys[memory->Vx[x]])
                memory->pc += 2;
        }
        else if (kk == 0xA1)
        {
            // SKPN Vx
            if (!keyboard->keys[memory->Vx[x]])
                memory->pc += 2;
        }
    }
    else if (instruction >> 12 == 0xF)
    {
        if (kk == 0x07)
        {
            // LD Vx, DT
            sem_wait(&delay_sem);
            memory->Vx[x] = memory->delay_timer;
            sem_post(&delay_sem);
        }
        else if (kk == 0x0A)
        {
            // LD Vx, K
            
            int key_index = -1;
            SDL_Event event;
            while (key_index == -1)
            {
                while (SDL_WaitEvent(&event))
                {
                    if (event.type == SDL_KEYDOWN)
                    {
                        int key = keyboard->process_key(event, false);
                        if (key != -1)
                        {
                            key_index = key;
                            break;
                        }
                    }
                }
            }

            memory->Vx[x] = key_index;
        }
        else if (kk == 0x15)
        {
            // Fx15 - LD DT, Vx
            sem_wait(&delay_sem);
            memory->delay_timer = memory->Vx[x];
            sem_post(&delay_sem);
        }
        else if (kk == 0x18)
        {
            // Fx18 - LD ST, Vx
            sem_wait(&sound_sem);
            memory->sound_timer = memory->Vx[x];
            sem_post(&sound_sem);
        }
        else if (kk == 0x1E)
        {
            // ADD I, Vx
            memory->I += memory->Vx[x];
        }
        else if (kk == 0x29)
        {
            // LD F, Vx
            memory->I = memory->Vx[x] * 5;
        }
        else if (kk == 0x33)
        {
            // Fx33 - LD B, Vx
            unsigned char hundreds = memory->Vx[x] / 100;
            unsigned char tens = (memory->Vx[x] % 100) / 10;
            unsigned char ones = memory->Vx[x] % 100 % 10;

            memory->main_memory[memory->I] = hundreds;
            memory->main_memory[memory->I + 1] = tens;
            memory->main_memory[memory->I + 2] = ones;
        }
        else if (kk == 0x55)
        {
            // LD [I], Vx
            for (int i = 0; i <= x; i++)
            {
                // memory->main_memory[memory->I + i] = memory->Vx[i];
                memory->main_memory[memory->I++] = memory->Vx[i];
            }
        }
        else if (kk == 0x65)
        {
            // Fx65 - LD Vx, [I]
            for (int i = 0; i <= x; i++)
            {
                // memory->Vx[i] = memory->main_memory[memory->I + i];
                memory->Vx[i] = memory->main_memory[memory->I++];
            }
        }
    }
    else
        printf("Not recognized!!!\n");
}

void Cpu::delay_timer()
{
    while (emulator_is_running)
    {
        sem_wait(&delay_sem);
        if (memory->delay_timer > 0)
        {
            usleep((1000 / 60) * 1000); // 60 Hz -> 1sec/60;
            memory->delay_timer--;
        }
        sem_post(&delay_sem);
    }
}

void Cpu::sound_timer()
{
    while (emulator_is_running)
    {
        sem_wait(&sound_sem);
        if (memory->sound_timer > 0)
        {
            Beeper::play();
            usleep(memory->sound_timer * 10000);
            Beeper::stop();
            memory->sound_timer = 0;
        }
        sem_post(&sound_sem);
    }
}
