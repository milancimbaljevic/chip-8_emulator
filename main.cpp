#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <curses.h>
#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include "constants.h"
#include "keyboard.h"
#include "cpu.h"
#include "memory.h"
#include "Beeper.h"

int emulator_is_running = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
int number_of_instructions_per_frame = 10; //10
Keyboard keyboard;
Memory memory;
Cpu cpu(&memory, &keyboard);
std::thread *sound_timer_thread;

int initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        "CHIP-8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}

void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
        {
            emulator_is_running = false;
            return;
        }

        switch (event.type)
        {
        case SDL_QUIT:
            emulator_is_running = false;
            break;
        default:
            keyboard.process_key(event);
            break;
        }
    }
}

void render(void)
{
    for (int i = 0; i < MACHINE_HEIGHT; i++)
    {
        for (int j = 0; j < MACHINE_WIDTH; j++)
        {
            const SDL_Rect pixel = {
                (int)j * PIXEL_SIZE,
                (int)i * PIXEL_SIZE,
                (int)PIXEL_SIZE,
                (int)PIXEL_SIZE};

            if (cpu.display[i][j])
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pixel);
            }
            else
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char *args[])
{
    if (argc <= 1)
    {
        printf("Error: You must specify a ROM file.\n");
        exit(-1);
    }

    char *rom_name = args[1];
    printf("%s\n", rom_name);

    emulator_is_running = initialize_window();
    memory.pc = PROGRAM_START;
    memory.delay_timer = 0;

    const double c4 = 261.63;
    
    Beeper::open();
    Beeper::setVolume(1.0);
    Beeper::setFrequency(c4);

    sem_init(&cpu.delay_sem, 0, 1);
    sem_init(&cpu.sound_sem, 0, 1);

    sound_timer_thread = new std::thread(&Cpu::sound_timer, &cpu);

    cpu.load_rom(rom_name);

    while (emulator_is_running)
    {
        process_input();

        const uint64_t exec_start = SDL_GetPerformanceCounter();

        for (uint32_t i = 0; i < number_of_instructions_per_frame; i++)
            cpu.fetch_decode_execute();

        const uint64_t exec_end = SDL_GetPerformanceCounter();
        const double time_elapsed = (double)((exec_end - exec_start) * 1000) / SDL_GetPerformanceFrequency();

        SDL_Delay(16.67f > time_elapsed ? 16.67f - time_elapsed : 0);

        if (cpu.screen_modified)
        {
            render();
            cpu.screen_modified = false;
        }

        if (memory.delay_timer > 0)
        {
            memory.delay_timer--;
        }
    }

    sound_timer_thread->join();
    delete sound_timer_thread;
    Beeper::close();
    destroy_window();

    std::string cmd = "launcher";
    
   int pid = fork();
   
   if(pid == 0){
	execl(cmd.c_str(), cmd.c_str(), NULL);
   }

    return 0;
}



