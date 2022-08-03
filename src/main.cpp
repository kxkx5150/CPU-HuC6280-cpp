#define SDL_MAIN_HANDLED

#include "pce.h"
#include <cstddef>
#include <cstdint>
#include <SDL2/SDL.h>
#include <cstdio>
#include <time.h>
#include <thread>
#include <bitset>

int Running = 1;
int width = 342, height = 262;

void render_loop(PC *pc, SDL_Renderer *render, int width, int height)
{
    while (Running) {
        // pc->paint(render, width, height);
    }
}
int main(int ArgCount, char **Args)
{
    PC *pc = new PC();
    pc->init();

    SDL_Renderer *renderer;
    SDL_Texture  *MooseTexture;
    SDL_bool      done = SDL_FALSE;
    SDL_Window   *window =
        SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width * 2, height * 2, SDL_WINDOW_SHOWN);
    renderer     = SDL_CreateRenderer(window, -1, 0);
    MooseTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    SDL_RenderSetScale(renderer, 2.0, 2.0);
    SDL_Event event;
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        exit(EXIT_FAILURE);
    atexit(SDL_Quit);
    pc->DrawFlag = false;

    pc->start(renderer, MooseTexture, width, height);

    size_t  count = 0;
    size_t  i     = 0;
    bool    quit  = false;
    uint8_t pad   = 0;

    while (!quit) {
        i++;
        if (i == count)
            break;

        while (!pc->DrawFlag) {
            pc->run_cpu();
            if (pc->BRAKE)
                break;
        }

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN: {
                    auto keyCode = event.key.keysym.sym;
                    pc->io->CheckKeyDownFunction(keyCode);
                } break;
                case SDL_KEYUP: {
                    auto keyCode = event.key.keysym.sym;
                    pc->io->CheckKeyUpFunction(keyCode);
                } break;
            }
        }
        pc->DrawFlag = false;
    }
}
