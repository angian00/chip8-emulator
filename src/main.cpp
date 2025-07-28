#include <iostream>
#include <format>
#include <chrono>
using namespace std;


#include "memory.hpp"
#include "cpu.hpp"
#include "display.hpp"


Memory* memory;
Display* display;
Cpu* cpu;

bool emulateCycle();


const int targetFps = 60;
const int frameDelay = 1000 / targetFps;


int main(int argc, char* argv[])
{
    cout << "CHIP-8 emulator by AnGian" << endl;

    if (argc < 2) {
        cout << "Missing ROM path" << endl;
        exit(1);
    }

    memory = new Memory();
    auto romOk = memory->load(argv[1]);
    if (!romOk) {
        cout << "Error loading ROM" << endl;
        return 1;
    }

    display = new Display();
    auto sdlOk = display->initSdl();
    if (!sdlOk) {
        cout << "Error initializing SDL" << endl;
        display->shutdownSdl();
        return 1;
    }
    
    cpu = new Cpu(memory, display);

    //DEBUG
    //memory->dump();
    //exit(0);
    //

    bool running = true;
    while (running) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        emulateCycle();
        
        display->refresh();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();

        if (frameDelay > frameTime) {
            display->delay(frameDelay - frameTime);
        }

        running = display->handleEvents();
        if (!running)
            cout << "Got SDL quit" << endl;
    }

    display->shutdownSdl();
    return 0;
}


bool emulateCycle() {
    auto pc = memory->getPC();
    if (memory->getPC() >= Memory::MAX_MEMORY_SIZE) {
        //cout << "Out of Memory!!" << endl;
        //break;
        return false;

    } else {
        auto instr = memory->fetch();

        //Decode the instruction to find out what the emulator should do
        // cout << " pc: 0x" << std::format("{:04x}", pc)
        // << " instr: " << std::format("{:04x}", instr)
        // << endl;
        
        //running = cpu->execute(instr);
       return cpu->execute(instr);
    }
}

