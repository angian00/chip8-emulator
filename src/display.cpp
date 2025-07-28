#include "display.hpp"

#include <iostream>
#include <format>
using namespace std;


#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

//64 pixels wide and 32 tall


const uint8_t Display::COLOR_BLACK[] = { 0x00, 0x00, 0x00, 0xff };
const uint8_t Display::COLOR_WHITE[] = { 0xcc, 0xcc, 0xcc, 0xff };

bool Display::initSdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "SDL could not be initialized!" << endl
             << "SDL_Error: " << SDL_GetError() << endl;
        return false;
    }
    sdlInited = true;

    auto screenWidth = WIDTH * SCALE_FACTOR;
    auto screenHeight = HEIGHT * SCALE_FACTOR;

    window = SDL_CreateWindow("CHIP-8 Emulator by AnGian",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          screenWidth, screenHeight,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        cout << "Window could not be created!" << endl
             << "SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        cout << "Renderer could not be created!" << endl
             << "SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    clear();

    return true;
}

void Display::shutdownSdl()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);

    if (window)
        SDL_DestroyWindow(window);

    if (sdlInited)
        SDL_Quit();
}


void Display::clear()
{
    SDL_SetRenderDrawColor(renderer, COLOR_BLACK[0], COLOR_BLACK[1], COLOR_BLACK[2], COLOR_BLACK[3]);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}


void Display::togglePixel(uint8_t x, uint8_t y)
{
    pixels[x][y] = !(pixels[x][y]);
    
    uint8_t* newColor;
    if (pixels[x][y])
        newColor = (uint8_t *)COLOR_WHITE;
    else
        newColor = (uint8_t *)COLOR_BLACK;

    // cout << "After setting newColor" << endl;

    SDL_SetRenderDrawColor(renderer, newColor[0], newColor[1], newColor[2], newColor[3]);
    // cout << "After SDL_SetRenderDrawColor" << endl;

    SDL_Rect rect(x*SCALE_FACTOR, y*SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);
    SDL_RenderFillRect(renderer, &rect);
    // cout << "After SDL_RenderFillRect" << endl;

    SDL_RenderPresent(renderer);
    // cout << "After SDL_RenderPresent" << endl;
}

void Display::refresh()
{
    SDL_RenderPresent(renderer);
    SDL_RaiseWindow(window);
}

void Display::delay(int delay)
{
    SDL_Delay(delay);
}

bool Display::handleEvents()
{
    SDL_Event e;

    auto hasEvent = SDL_PollEvent(&e);
    return !(hasEvent && e.type == SDL_QUIT);
}

