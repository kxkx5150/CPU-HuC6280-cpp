#include "pce.h"
#include <cstdint>

PC::PC()
{
}
PC::~PC()
{
    delete mem;
    delete cpu;
    delete mapper;
    delete irq;
    delete timer;
    delete vpc;
    delete vce;
    delete vdc;
}
void PC::init()
{
    mem = new Mem(this);
    load("pce_test.pce");
    cpu   = new CPU(mem);
    irq   = new IRQ(this);
    timer = new TIMER(this);
    vpc   = new VPC();
    vce   = new VCE(this);
    vdc   = new VDC(this);

    reset();
}
void PC::reset()
{
    irq->Reset();
    mem->StorageReset();
    cpu->CPUInit();
    vdc->VDCInit();
    timer->TimerInit();
    cpu->CPUReset();
}
void PC::load(std::string path)
{
    FILE *f = fopen(path.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    const int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *buffer = new uint8_t[size];
    auto     _      = fread(buffer, size, 1, f);

    int      stat   = size % 8192;
    int      romlen = size - stat;
    uint8_t *rom    = new uint8_t[romlen];

    for (int64_t i = stat; i < size; i++) {
        rom[i - stat] = buffer[i];
    }
    mapper = new Mapper0(rom, romlen);
    fclose(f);
    delete[] buffer;
}
void PC::start(SDL_Renderer *_renderer, SDL_Texture *_MooseTexture, int _width, int _height)
{
    width  = _width;
    height = _height;

    renderer     = _renderer;
    MooseTexture = _MooseTexture;
}
void PC::run_cpu()
{
    cpu->CPURun();
    vdc->VDCRun();
    timer->TimerRun();

    if (DrawFlag) {
    }
}
void PC::put_image(uint32_t *imgdata)
{
    int64_t start = SDL_GetPerformanceCounter();

    UpdateTexture(MooseTexture, imgdata, width, height);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, MooseTexture, NULL, NULL);
    SDL_RenderPresent(renderer);

    Uint64 end       = SDL_GetPerformanceCounter();
    float  elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
    if (16.666f > elapsedMS)
        SDL_Delay(floor(16.666f - elapsedMS));
    // DrawFlag = false;
}
void PC::UpdateTexture(SDL_Texture *texture, uint32_t *imgdata, int width, int height)
{
    size_t     imgidx = 0;
    SDL_Color *color;
    Uint8     *src;
    Uint32    *dst;
    int        row, col;
    void      *pixels;
    int        pitch;
    SDL_LockTexture(texture, NULL, &pixels, &pitch);

    for (row = 0; row < height; ++row) {
        dst = (Uint32 *)((Uint8 *)pixels + row * pitch);
        for (col = 0; col < width; ++col) {
            *dst++ = imgdata[imgidx++];
        }
    }
    SDL_UnlockTexture(texture);
}
