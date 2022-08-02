#ifndef VCE_H
#define VCE_H
#include <cstdint>

struct RGB
{
    int64_t r = 0;
    int64_t g = 0;
    int64_t b = 0;
};

class PC;
class VCE {
  public:
    PC *Core = nullptr;

    int64_t VCEBaseClock = 0;
    int64_t VCEControl   = 0;
    int64_t VCEAddress   = 0;
    int64_t VCEData      = 0;

    int64_t Palette[512];
    RGB      PaletteData[512];

  public:
    VCE(PC *_core);

    void     SetVCEControl(int64_t data);
    void     SetVCEAddressLow(int64_t data);
    void     SetVCEAddressHigh(int64_t data);
    int64_t GetVCEDataLow();
    int64_t GetVCEDataHigh();
    void     SetVCEDataLow(int64_t data);
    void     SetVCEDataHigh(int64_t data);
    void     ToPalettes();
};
#endif