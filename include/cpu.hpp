#pragma once

#include "memory.hpp"
#include "display.hpp"
#include "keyboard.hpp"

class Cpu
{
    public:
        Cpu(Memory* memory, Display* display, Keyboard* keyboard);

        void updateTimers();
        bool execute(uint16_t instr);
        
    private:
        Memory* memory;
        Display* display;
        Keyboard* keyboard;

        uint8_t delayTimer;
        //uint8_t soundTimer;
};