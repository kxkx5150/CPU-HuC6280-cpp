#include "vce.h"
#include "pce.h"

VCE::VCE(PC *_core)
{
    Core         = _core;
    VCEBaseClock = Core->cpu->BaseClock5;
    VCEControl   = 0x00;
    VCEAddress   = 0x00;
    VCEData      = 0x00;
}

void VCE::SetVCEControl(int64_t data)
{
    VCEControl = data;
    switch (data & 0x03) {
        case 0x00:
            VCEBaseClock = Core->cpu->BaseClock5;
            break;
        case 0x01:
            VCEBaseClock = Core->cpu->BaseClock7;
            break;
        case 0x02:
        case 0x03:
            VCEBaseClock = Core->cpu->BaseClock10;
            break;
    }
}
void VCE::SetVCEAddressLow(int64_t data)
{
    VCEAddress = (VCEAddress & 0xff00) | data;
}
void VCE::SetVCEAddressHigh(int64_t data)
{
    VCEAddress = ((VCEAddress & 0x00ff) | (data << 8)) & 0x01ff;
}
int64_t VCE::GetVCEDataLow()
{
    return Palette[VCEAddress] & 0x00ff;
}
int64_t VCE::GetVCEDataHigh()
{
    int64_t tmp = (Palette[VCEAddress] & 0xff00) >> 8;
    VCEAddress   = (VCEAddress + 1) & 0x01ff;
    return tmp;
}
void VCE::SetVCEDataLow(int64_t data)
{
    Palette[VCEAddress] = (Palette[VCEAddress] & 0xff00) | data;
    ToPalettes();
}
void VCE::SetVCEDataHigh(int64_t data)
{
    Palette[VCEAddress] = (Palette[VCEAddress] & 0x00ff) | (data << 8);
    ToPalettes();
    VCEAddress = (VCEAddress + 1) & 0x01ff;
}
void VCE::ToPalettes()
{
    int64_t color            = Palette[VCEAddress];
    PaletteData[VCEAddress].r = ((color >> 3) & 0x07) * 36;
    PaletteData[VCEAddress].g = ((color >> 6) & 0x07) * 36;
    PaletteData[VCEAddress].b = (color & 0x07) * 36;
}