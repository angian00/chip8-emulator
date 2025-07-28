#pragma once

#include "memory.hpp"
#include "display.hpp"

class Cpu
{
    public:
        Cpu(Memory* memory, Display* display): memory(memory), display(display) {}

        bool execute(uint16_t instr);
        
    private:
        Memory* memory;
        Display* display;
};