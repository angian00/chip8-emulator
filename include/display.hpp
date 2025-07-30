#pragma once

#include <SDL.h>

class Display {
    public:
        static const int WIDTH = 64;
        static const int HEIGHT =32;
        static const int SCALE_FACTOR = 20;

        static const uint8_t COLOR_BLACK[];
        static const uint8_t COLOR_WHITE[];

        Display() {}

        bool initSdl();
        void shutdownSdl();

        void run();

        void clear();
        void togglePixel(uint8_t x, uint8_t y);

        void refresh();
        void delay(int delay);
        
    private:
        bool pixels[WIDTH][HEIGHT];
        
        bool sdlInited = false;
        SDL_Window* window;
        SDL_Renderer* renderer;

};
