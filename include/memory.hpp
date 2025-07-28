#pragma once

#include <stdint.h>

enum
{
    V0=0,
    V1,
};


class Memory {
    public:
        static const int MAX_MEMORY_SIZE = 4096;
        static const int MAX_STACK_SIZE = 1024;
        
        Memory() {}

        uint16_t getRI() { return ri; };
        void setRI(uint16_t value) { ri = value; };
        uint16_t getPC() { return pc; };
        void setPC(uint16_t value) { pc = value; };
        uint8_t getRegister(uint8_t registerIndex) { return rv[registerIndex]; };
        void setRegister(uint8_t registerIndex, uint8_t value) { rv[registerIndex] = value; };

        uint8_t getMemory(uint16_t address) { return memory[address]; };
        void setMemory(uint16_t address, uint8_t value) { memory[address] = value; };


        bool load(const char * file_path);
        uint16_t fetch();
        void dump();

    private:
        uint8_t memory[MAX_MEMORY_SIZE];
        uint8_t stack[MAX_STACK_SIZE];

        uint16_t ri;
        uint16_t pc;
        uint8_t  rv[16];
};
