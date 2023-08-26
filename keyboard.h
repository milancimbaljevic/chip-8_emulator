#pragma once

#include "constants.h"
#include <SDL2/SDL.h>

class Keyboard{
public:
    bool keys[NUMBER_OF_KEYS] = { false }; // true is key is down, false if key is up
    int process_key(SDL_Event& event, bool notify = true);
};