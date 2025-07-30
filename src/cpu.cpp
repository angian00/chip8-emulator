#include "cpu.hpp"

#include <iostream>
#include <format>
#include <cstdlib>
#include <ctime>

using namespace std;

#define BITSHIFT_IN_PLACE 1


Cpu::Cpu(Memory* memory, Display* display, Keyboard* keyboard): 
        memory(memory), display(display), keyboard(keyboard)
{
    delayTimer = 0x00;
    srand(time(0));
}


void Cpu::updateTimers()
{
    if (delayTimer > 0)
        delayTimer --;
}

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
                    //clearScreen
                    display->clear();
                    break;

                case 0x00EE:
                    //returnSubroutine
                    memory->setPC(memory->stackPop());
                    break;

                default:
                    //originally "Execute machine language routine"
                    cout << "Skipping unsupported opcode: " << format("{:04x}", instr) << endl;
                    return false;
            }
            break;

        case 0x1:
            //jump(nnn)
            memory->setPC(nnn);
            break;

        case 0x2:
            //callSubroutine
            memory->stackPush(memory->getPC());
            memory->setPC(nnn);
            break;

        case 0x3:
            //skipIfEqual(x,nn)
            if (memory->getRegister(x) == nn)
                memory->setPC(memory->getPC() + 2);
            break;

        case 0x4:
            //skipIfNotEqual(x,nn)
            if (memory->getRegister(x) != nn)
                memory->setPC(memory->getPC() + 2);
            break;

        case 0x5:
            //skipIfRegisterEqual(x,y)
            if (memory->getRegister(x) == memory->getRegister(y))
                memory->setPC(memory->getPC() + 2);
            break;

        case 0x6:
            //setRegisterVX(x, nn)
            memory->setRegister(x, nn);
            break;

        case 0x7:
            //addToRegisterVX(x, nn)
            memory->setRegister(x, memory->getRegister(x) + nn);
            break;

        case 0x8:
            switch (n) {
                case 0x0:
                    //assign(x, y)
                    memory->setRegister(x, memory->getRegister(y));
                    break;

                case 0x1:
                    //bitwiseOr(x, y)
                    memory->setRegister(x, memory->getRegister(x) | memory->getRegister(y));
                    break;

                case 0x2:
                    //bitwiseAnd(x, y)
                    memory->setRegister(x, memory->getRegister(x) & memory->getRegister(y));
                    break;

                case 0x3:
                    //bitwiseXor(x, y)
                    memory->setRegister(x, memory->getRegister(x) ^ memory->getRegister(y));
                    break;

                case 0x4:
                {
                    //add(x, y)
                    uint16_t sum = memory->getRegister(x) + memory->getRegister(y);
                    memory->setRegister(0xf, (sum > 0xff) ? 1 : 0); //flag register
                    memory->setRegister(x, sum & 0xff);
                    break;
                }

                case 0x5:
                {
                    //subtract(x, y)
                    //notice that carry flag behaviour is counter-intuitive
                    int8_t diff = memory->getRegister(x) - memory->getRegister(y);
                    memory->setRegister(0xf, (diff > 0) ? 1 : 0); //flag register
                    memory->setRegister(x, diff);
                    break;
                }

                case 0x6:
                {
                    //bitShiftRight(x)
                    // NB: real behaviour varies by chipset
                    if (!BITSHIFT_IN_PLACE)
                        memory->setRegister(x, memory->getRegister(y));
                    memory->setRegister(0xf, memory->getRegister(x) & 0x01); //flag register
                    memory->setRegister(x, memory->getRegister(x) >> 1);
                    break;
                }

                case 0x7:
                {
                    //reverseSubtract(x, y)
                    //notice that carry flag behaviour is counter-intuitive
                    int8_t diff = memory->getRegister(y) - memory->getRegister(x);
                    memory->setRegister(0xf, (diff > 0) ? 1 : 0); //flag register
                    memory->setRegister(x, diff);
                    break;
                }

                case 0xE:
                {
                    //bitShiftLeft(x)
                    // NB: real behaviour varies by chipset
                    if (!BITSHIFT_IN_PLACE)
                        memory->setRegister(x, memory->getRegister(y));
                    memory->setRegister(0xf, memory->getRegister(x) & 0x80); //flag register
                    memory->setRegister(x, memory->getRegister(x) << 1);
                    break;
                }

                default:
                    cout << "Invalid 8XYN opcode: " << format("{:04x}", instr) << endl;
                    return false;                
            }
            break;

        case 0x9:
            //skipIfRegisterNotEqual(x,y)
            if (memory->getRegister(x) != memory->getRegister(y))
                memory->setPC(memory->getPC() + 2);
            break;

        case 0xA:
            //setIndexRegister(nnn)
            memory->setRI(nnn);
            break;

        case 0xB:
            //jumpWithOffset(nnn)
            // NB: real behaviour varies by chipset
            // see https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#bnnn-jump-with-offset
            memory->setPC(nnn + memory->getRegister(0x0));
            break;

        case 0xC:
        {
            //random(x, nn)
            uint8_t val = rand() % 0x100;
            val = (val & nn);

            memory->setRegister(x, val);
            break;
        }

        case 0xD:
        {
            //draw(x, y, n)
            //see https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#dxyn-display
            uint8_t xVal = memory->getRegister(x) % 64;
            uint8_t yVal = memory->getRegister(y) % 32;
            memory->setRegister(0xf, 0x00); //flag register
            
            uint16_t ri = memory->getRI();
            
            // cout << "now drawing; xVal: " << format("{:d}", xVal) << ", yVal: " << format("{:d}", yVal)
            //      << " ri: " << ri << endl;
            auto ySprite = 0;
            while ((ySprite < n) && (yVal + ySprite < Display::HEIGHT))
            {
                auto spriteData = memory->getMemory(ri + ySprite);
                //cout << "spriteData: 0x" << format("{:02x}", spriteData) << endl;
                auto xSprite = 0;
                while ((xSprite < 8) && (xVal + xSprite < Display::WIDTH))
                {
                    bool spritePixel = (spriteData >> (7 - xSprite)) & 0x1;
                    if (spritePixel)
                        display->togglePixel(xVal + xSprite, yVal + ySprite);

                    xSprite ++;
                }

                ySprite ++;
            }
            break;
        }   

        case 0xE:
            switch (nn)
            {
                case 0x9E:
                {
                    //skipIfKeyPressed(x)
                    auto targetKeyValue = memory->getRegister(x);
                    auto pressedKey = keyboard->getKey();
                    if (pressedKey == targetKeyValue)
                        memory->setPC(memory->getPC() + 2);
                    break;
                }
                
                case 0xA1:
                {
                    //skipIfKeyNotPressed(x)
                    auto targetKeyValue = memory->getRegister(x);
                    auto pressedKey = keyboard->getKey();
                    if (!(pressedKey == targetKeyValue))
                        memory->setPC(memory->getPC() + 2);
                    break;
                }

                default:
                    cout << "Invalid EXNN opcode: " << format("{:04x}", instr) << endl;
                    return false;                

            }
            break;

        case 0xF:
            switch (nn)
            {
                case 0x07:
                    //getDelayTimer(x)
                    memory->setRegister(x, delayTimer);
                    break;

                case 0x0A:
                {
                    //getKey(x)
                    auto pressedKey = keyboard->getKey();
                    if (pressedKey == NoKey)
                    {
                        //do again
                        memory->setPC(memory->getPC() - 2);
                    }
                    else
                    {
                        memory->setRegister(x, pressedKey);
                    }

                    break;
                }

                case 0x15:
                    //setDelayTimer(x)
                    delayTimer = memory->getRegister(x);
                    break;

                case 0x18:
                    //setSoundTimer(x)
                    //TODO
                    cout << "TODO: setSoundTimer" << endl;
                    break;

                case 0x1E:
                {
                    //addToRegisterIndex(x)
                    // NB: real overflow flag behaviour varies by chipset
                    // see https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx1e-add-to-index
                    //some interpreters set VF to 1 if I “overflows” from 0FFF to above 1000 (outside the normal addressing range).
                    uint16_t newRI = memory->getRI() + memory->getRegister(x);
                    if (newRI > 0x1000)
                        memory->setRegister(0xf, 0x01); //flag overflow in VF

                    memory->setRI(newRI);
                    break;
                }
                case 0x29:
                    //setRegisterIndex(x)
                    memory->setRI(memory->getRegister(x) & 0x0f);
                    break;

                case 0x33:
                {
                    //convertToDecimal(x)
                    auto destAddr = memory->getRI() + 2;
                    auto xVal = memory->getRegister(x);
                    memory->setMemory(destAddr, xVal % 10);

                    xVal /= 10;
                    destAddr --;
                    memory->setMemory(destAddr, xVal % 10);

                    xVal /= 10;
                    destAddr --;
                    memory->setMemory(destAddr, xVal % 10);

                    break;
                }

                case 0x55:
                {
                    //dumpRegisters(x)
                    // NB: whether RI should be modified varies by chipset
                    // see https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx55-and-fx65-store-and-load-memory
                    auto ri = memory->getRI();
                    for (auto iReg=0; iReg <= x; iReg++)
                    {
                        memory->setMemory(ri + iReg, memory->getRegister(iReg));
                    }
                    break;
                }

                case 0x65:
                {
                    //restoreRegisters(x)
                    // NB: whether RI should be modified varies by chipset
                    // see https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#fx55-and-fx65-store-and-load-memory

                    auto ri = memory->getRI();
                    for (auto iReg=0; iReg <= x; iReg++)
                    {
                        memory->setRegister(iReg, memory->getMemory(ri + iReg));
                    }
                    break;
                }

                default:
                    cout << "Invalid FXNN opcode: " << format("{:04x}", instr) << endl;
                    return false;                

            }
            break;

        default:
            cout << "Unsupported opcode: " << format("{:04x}", instr) << endl;
            return false;
    }

    return true;
}


