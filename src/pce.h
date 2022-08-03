#ifndef PC_H
#define PC_H

#include <cstdint>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "cpu.h"
#include "mem.h"
#include "mapper.h"
#include "irq.h"
#include "timer.h"
#include "vpc.h"
#include "vce.h"
#include "vdc.h"
#include "io.h"

class PC {
  public:
    CPU     *cpu    = nullptr;
    Mem     *mem    = nullptr;
    Mapper0 *mapper = nullptr;
    IRQ     *irq    = nullptr;
    TIMER   *timer  = nullptr;
    VPC     *vpc    = nullptr;
    VCE     *vce    = nullptr;
    VDC     *vdc    = nullptr;
    IO      *io     = nullptr;

    bool BRAKE    = false;
    bool DrawFlag = false;

    int64_t MainCanvas_width  = 320;
    int64_t MainCanvas_height = 262;

    int width  = 0;
    int height = 0;

    SDL_Renderer *renderer     = nullptr;
    SDL_Texture  *MooseTexture = nullptr;

  public:
    PC();
    ~PC();

    void init();
    void reset();

    void load(std::string path);
    void start(SDL_Renderer *_renderer, SDL_Texture *_MooseTexture, int _width, int _height);
    void run_cpu();
    void UpdateTexture(SDL_Texture *texture, uint32_t *imgdata, int width, int height);
    void put_image(uint32_t *imgdata);
};
#endif
