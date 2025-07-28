#include "memory.hpp"

#include <iostream>
#include <format>
using namespace std;


#define START_ADDRESS 0x200

bool Memory::load(const char *file_path)
{
    FILE *f;
    if (!(f = fopen(file_path, "rb")))
    {
        cout << file_path << " is not a readable file" << endl;
        return false;
    }

    auto n_read = fread(memory + START_ADDRESS, sizeof(uint8_t), MAX_MEMORY_SIZE, f);    
    fclose(f);

    pc = START_ADDRESS;
    return true;
}

uint16_t Memory::fetch()
{
    if (pc >= MAX_MEMORY_SIZE)
    {
        cout << "Out of Memory Error!!" << endl;
        return 0x00;
    }

    uint16_t instr = 0x0000;
    instr |= memory[pc++] << 8;
    instr |= memory[pc++];

    return instr;
}


void Memory::dump()
{
    for (auto i=0; i < MAX_MEMORY_SIZE; i++)
    {
        cout << "0x" << std::format("{:04x}", i)
             << " " << std::format("{:02x}", memory[i])
             << endl;
    }   
}
