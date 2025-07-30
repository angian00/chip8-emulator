#pragma once

#include <SDL.h>

const uint8_t N_KEYS = 0x10;
typedef uint8_t KeyValue;
const KeyValue NoKey = 0xff;


class Keyboard {
    public:
        bool handleEvents();

        KeyValue getKey();
        
    private:
        bool keysDown[N_KEYS];
};
