#include <iostream>
#include <format>
using namespace std;


#include "memory.hpp"
#include "cpu.hpp"
#include "display.hpp"


int main(int argc, char* argv[])
{
    cout << "CHIP-8 emulator by AnGian" << endl;

    if (argc < 2) {
        cout << "Missing ROM path" << endl;
        exit(1);
    }

    auto memory = new Memory();
    //TODO: get rom
    auto romOk = memory->load(argv[1]);
    if (!romOk) {
        cout << "Error loading ROM" << endl;
        return 1;
    }

    auto display = new Display();
    auto sdlOk = display->initSdl();
    if (!sdlOk) {
        cout << "Error initializing SDL" << endl;
        display->shutdownSdl();
        return 1;
    }
    
    auto cpu = new Cpu(memory, display);

    //DEBUG
    //memory->dump();
    //exit(0);
    //

    bool running = true;
    while (running) {
        //Fetch the instruction from memory at the current PC (program counter)
        auto pc = memory->getPC();
        if (memory->getPC() >= Memory::MAX_MEMORY_SIZE) {
            //cout << "Out of Memory!!" << endl;
            //break;
        } else {
            auto instr = memory->fetch();

            //Decode the instruction to find out what the emulator should do
            // cout << " pc: 0x" << std::format("{:04x}", pc)
            // << " instr: " << std::format("{:04x}", instr)
            // << endl;
            
            //running = cpu->execute(instr);
            cpu->execute(instr);
        }

        display->refresh();

        running = display->pollEvent();
        if (!running)
            cout << "Got SDL quit" << endl;
    }

    display->shutdownSdl();
    return 0;
}
