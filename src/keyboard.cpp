#include "keyboard.hpp"

#include <SDL.h>

#include <iostream>
#include <format>
using namespace std;


const SDL_Keycode KEYMAP[] = {
    SDLK_x, //0x0
    SDLK_1, //0x1
    SDLK_2, //0x2
    SDLK_3, //0x3
    SDLK_q, //0x4
    SDLK_w, //0x5
    SDLK_e, //0x6
    SDLK_a, //0x7
    SDLK_s, //0x8
    SDLK_d, //0x9
    SDLK_z, //0xA
    SDLK_c, //0xB
    SDLK_4, //0xC
    SDLK_r, //0xD
    SDLK_f, //0xE
    SDLK_v, //0xF
};

KeyValue mapKey(SDL_Keycode sdlKeyId);


bool Keyboard::handleEvents()
{
    SDL_Event e;

    auto hasEvent = SDL_PollEvent(&e);
    if (!hasEvent)
        return true;

    switch (e.type) {
        case SDL_QUIT:
            return false;

        case SDL_KEYDOWN:
        {
            auto keyValue = mapKey(e.key.keysym.sym);
            if (keyValue != NoKey)
                keysDown[keyValue] = true;
            break;
        }

        case SDL_KEYUP:
        {
            auto keyValue = mapKey(e.key.keysym.sym);
            if (keyValue != NoKey)
                keysDown[keyValue] = false;
            break;
        }
    }

    return true;
}


KeyValue Keyboard::getKey() {
    for (auto keyValue=0; keyValue < N_KEYS; keyValue++)
    {
        if (keysDown[keyValue])
        {
            //keysDown[keyValue] = false;
            return keyValue;
        }
    }

    return NoKey;
}


KeyValue mapKey(SDL_Keycode sdlKeyId)
{
    for (auto keyValue=0; keyValue < N_KEYS; keyValue++)
    {
        if (KEYMAP[keyValue] == sdlKeyId)
            return keyValue;
    }

    //cout << "Unmapped key: " << sdlKeyId << endl;
    return NoKey;
}
