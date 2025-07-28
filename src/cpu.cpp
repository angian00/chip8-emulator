#include "cpu.hpp"

#include <iostream>
#include <format>
using namespace std;


bool Cpu::execute(uint16_t instr)
{
    uint8_t  opType  = instr >> 12;
    uint8_t  x   = (instr >> 8) & 0x0f;
    uint8_t  y   = (instr >> 4) & 0x0f;
    uint8_t  n   = (instr)      & 0x0f;
    uint8_t  nn  = (instr)      & 0xff;
    uint16_t nnn = (instr)      & 0xfff;

    switch (opType) {
        case 0x0:
            switch (instr)
            {
                case 0x00E0:
                    //clearScreen();
                    display->clear();
                    break;

                default:
                    //cout << "Unsupported opcode: " << format("{:04x}", instr) << endl;
                    return false;
            }
            break;

        case 0x1:
            //jump(nnn);
            memory->setPC(nnn);
            break;

        case 0x6:
            //setRegisterVX(x, nn);
            memory->setRegister(x, nn);
            break;

        case 0x7:
            //addToRegisterVX(x, nn);
            memory->setRegister(x, memory->getRegister(x) + nn);
            break;

        case 0xA:
            //setIndexRegister(nnn);
            cout << "setRI; nnn: " << format("{:d}", nnn) << endl;

            memory->setRI(nnn);
            break;

        case 0xD:
        {
            //draw(x, y, n);
            //implemented as per https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#dxyn-display
            uint8_t xVal = memory->getRegister(x) % 64;
            uint8_t yVal = memory->getRegister(y) % 32;
            memory->setRegister(0xf, 0x00); //flag register
            
            uint16_t ri = memory->getRI();
            
            // cout << "now drawing; xVal: " << format("{:d}", xVal) << ", yVal: " << format("{:d}", yVal)
            //      << " ri: " << ri << endl;
            for (auto iRow=0; iRow < n; iRow++)
            {
                auto spriteData = memory->getMemory(ri + iRow);
                //cout << "spriteData: 0x" << format("{:02x}", spriteData) << endl;
                auto xSprite = 0;
                while ((xSprite < 8) && (xVal + xSprite < Display::WIDTH))
                {
                    bool spritePixel = (spriteData >> (7 - xSprite)) & 0x1;
                    if (spritePixel)
                        display->togglePixel(xVal + xSprite, yVal + iRow);

                    xSprite ++;
                }

            }

            break;
        }   

        default:
            //cout << "Unsupported opcode: " << format("{:04x}", instr) << endl;
            return false;
    }

    return true;
}

