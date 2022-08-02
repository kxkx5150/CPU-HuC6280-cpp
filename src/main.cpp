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
uint8_t keycode_to_pad(SDL_Event event)
{
    auto key = event.key.keysym.sym;
    // switch (key) {
    //     case SDLK_x:
    //         return PAD_A;
    //     case SDLK_z:
    //         return PAD_B;
    //     case SDLK_a:
    //         return PAD_SELECT;
    //     case SDLK_s:
    //         return PAD_START;
    //     case SDLK_RIGHT:
    //         return PAD_R;
    //     case SDLK_LEFT:
    //         return PAD_L;
    //     case SDLK_UP:
    //         return PAD_U;
    //     case SDLK_DOWN:
    //         return PAD_D;
    //     default:
    //         return 0;
    // }
    return 0;
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
                case SDL_KEYDOWN:
                    pad |= keycode_to_pad(event);
                    // io->set_ctrlstat1(pad);
                    break;
                case SDL_KEYUP: {
                    pad &= ~keycode_to_pad(event);
                    // io->set_ctrlstat1(pad);
                    break;
                }
            }
        }
        pc->DrawFlag = false;
    }
}
