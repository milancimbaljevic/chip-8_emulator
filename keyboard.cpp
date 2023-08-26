#include "keyboard.h"

int Keyboard::process_key(SDL_Event &event, bool notify)
{
    int key_index = -1;

    switch (event.key.keysym.sym)
    {
    case SDLK_1:
        key_index = 0x1;
        break;
    case SDLK_2:
        key_index = 0x2;
        break;
    case SDLK_3:
        key_index = 0x3;
        break;
    case SDLK_4:
        key_index = 0xC;
        break;
    case SDLK_q:
        key_index = 0x4;
        break;
    case SDLK_w:
        key_index = 0x5;
        break;
    case SDLK_e:
        key_index = 0x6;
        break;
    case SDLK_r:
        key_index = 0xD;
        break;
    case SDLK_a:
        key_index = 0x7;
        break;
    case SDLK_s:
        key_index = 0x8;
        break;
    case SDLK_d:
        key_index = 0x9;
        break;
    case SDLK_f:
        key_index = 0xE;
        break;
    case SDLK_z:
        key_index = 0xA;
        break;
    case SDLK_x:
        key_index = 0x0;
        break;
    case SDLK_c:
        key_index = 0xB;
        break;
    case SDLK_v:
        key_index = 0xF;
        break;
    default:
        break;
    }

    if (key_index != -1 && notify)
    {
        if (event.type == SDL_KEYDOWN)
        {
            keys[key_index] = true;
        }
        else if (event.type == SDL_KEYUP)
        {
            keys[key_index] = false;
        }
    }

    return key_index;
}
