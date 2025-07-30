#include <iostream>
#include <format>
#include <chrono>
using namespace std;


#include "memory.hpp"
#include "cpu.hpp"
#include "display.hpp"
#include "keyboard.hpp"

//#define DEBUG_OPCODES 1
#define DEBUG_OPCODES 0


Memory* memory;
Cpu* cpu;

bool emulateCycle();


const int targetFps = 60;
const int frameDelay = 1000 / targetFps;
const int nCyclesPerFrame = 12;


int main(int argc, char* argv[])
{
    Display* display;
    Keyboard* keyboard;

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
    
    keyboard = new Keyboard();
    cpu = new Cpu(memory, display, keyboard);

    //DEBUG
    //memory->dump();
    //exit(0);
    //

    bool running = true;
    while (running) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        cpu->updateTimers();

        for (auto iCycle=0; iCycle < nCyclesPerFrame; iCycle++)
        {
            emulateCycle();
            display->refresh();
        }


        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto frameTime = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();

        if (frameDelay > frameTime) {
            display->delay(frameDelay - frameTime);
        }

        running = keyboard->handleEvents();
        if (!running)
            cout << "Got SDL quit" << endl;
    }

    display->shutdownSdl();
    return 0;
}


bool emulateCycle() {
    auto pc = memory->getPC();
    if (memory->getPC() >= Memory::MAX_MEMORY_SIZE) {
        cout << "Out of Memory!!" << endl;
        //break;
        return false;

    } else {
        auto instr = memory->fetch();

        if (DEBUG_OPCODES) {
            //Decode the instruction to find out what the emulator should do
            cout << " pc: 0x" << std::format("{:04x}", pc)
            << " instr: " << std::format("{:04x}", instr)
            << endl;
        }
        
       return cpu->execute(instr);
    }
}
