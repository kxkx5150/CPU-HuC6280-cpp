#include "vdc.h"
#include "pce.h"
#include <cstdint>
#include <cstdio>

VDC::VDC(PC *_core)
{
    Core    = _core;
    vdcp[0] = new VDCP();
    vdcp[1] = new VDCP();

    for (int i = 0; i < 12; i++) {
        ScreenSize[i]    = 0;
        PutScreenSize[i] = 0;
    }

    for (int i = 0; i < 0x30; i++) {
        VScreenWidthArray[i] = 0;
    }

    for (int i = 0; i < 0x100; i++) {
        int64_t b = (((i & 0x80) >> 7) | ((i & 0x40) >> 5) | ((i & 0x20) >> 3) | ((i & 0x10) >> 1) | ((i & 0x08) << 1) |
                     ((i & 0x04) << 3) | ((i & 0x02) << 5) | ((i & 0x01) << 7));

        int64_t rval256 =
            (((b & 0x80) << (28 - 7)) | ((b & 0x40) << (24 - 6)) | ((b & 0x20) << (20 - 5)) | ((b & 0x10) << (16 - 4)) |
             ((b & 0x08) << (12 - 3)) | ((b & 0x04) << (8 - 2)) | ((b & 0x02) << (4 - 1)) | ((b & 0x01) << (0 - 0)));
        ReverseBit[i]    = b;
        ReverseBit256[i] = rval256;
    }

    for (int i = 0; i < 0x10000; i++) {
        int64_t rval    = (ReverseBit[i & 0x00ff] << 8) | ReverseBit[(i & 0xff00) >> 8];
        ReverseBit16[i] = rval;
    }

    SPAddressMask[16][16] = 0x07fe;
    SPAddressMask[16][32] = 0x07fe & 0x07fa;
    SPAddressMask[16][64] = 0x07fe & 0x07f2;
    SPAddressMask[32][16] = 0x07fc;
    SPAddressMask[32][32] = 0x07fc & 0x07fa;
    SPAddressMask[32][64] = 0x07fc & 0x07f2;

    ScreenSize[Core->cpu->BaseClock5]     = 342;
    ScreenSize[Core->cpu->BaseClock7]     = 456;
    ScreenSize[Core->cpu->BaseClock10]    = 684;
    PutScreenSize[Core->cpu->BaseClock5]  = 320;
    PutScreenSize[Core->cpu->BaseClock7]  = 428;
    PutScreenSize[Core->cpu->BaseClock10] = 640;

    ScreenHeightMAX = 262;
    ScreenWidthMAX  = 684;

    VScreenWidthArray[0x00] = 32;
    VScreenWidthArray[0x10] = 64;
    VScreenWidthArray[0x20] = 128;
    VScreenWidthArray[0x30] = 128;
}
void VDC::VDCInit()
{
    VDCPutLineProgressClock = 0;
    VDCPutLine              = 0;
    for (int vdcno = 0; vdcno < 2; vdcno++) {
        vdcp[vdcno]->VDCRegister[0x09] = 0x0010;
        vdcp[vdcno]->VDCRegister[0x0a] = 0x0202;
        vdcp[vdcno]->VDCRegister[0x0b] = 0x031f;
        vdcp[vdcno]->VDCRegister[0x0c] = 0x0f02;
        vdcp[vdcno]->VDCRegister[0x0d] = 0x00ef;
        vdcp[vdcno]->VDCRegister[0x0e] = 0x0003;
    }
    GetScreenSize(0);
    GetScreenSize(1);
}
void VDC::VDCRun()
{
    VDCProcessDMA(0);
    vdcp[0]->VDCProgressClock += Core->cpu->ProgressClock;

    while (vdcp[0]->VDCProgressClock >= VDCLineClock) {
        VDCProcess(0);
    }
    VDCPutLineProgressClock += Core->cpu->ProgressClock;
    if (VDCPutLineProgressClock >= VDCLineClock) {
        VDCPutLineProgressClock -= VDCLineClock;
        VDCPutLine++;
        if (VDCPutLine == ScreenHeightMAX) {
            VDCPutLine = 0;
            GetScreenSize(0);
            vdcp[0]->DrawBGYLine = 4;
            Core->DrawFlag       = true;
            imgidx               = 0;
            Core->put_image(imgdata);
            counts++;
            if (counts == 14) {
                printf(" ");
            }
        }
        auto     palettes   = Core->vce->PaletteData;
        int64_t  imageIndex = VDCPutLine * ScreenWidthMAX * 4;
        int64_t  sw         = ScreenSize[Core->vce->VCEBaseClock];
        int64_t *bgl0       = vdcp[0]->BGLine;

        for (int bgx = 0; bgx < sw; bgx++) {
            RGB      color  = palettes[bgl0[bgx]];
            uint32_t dots   = (255 << 24) | (color.r << 16) | (color.g << 8) | color.b;
            imgdata[imgidx] = dots;
            imgidx++;

            // imgdata[imageIndex / 4]imgidx
        }
    }
}
void VDC::VDCProcess(int64_t vdcno)
{
    auto vdcc = vdcp[vdcno];
    vdcc->VDCProgressClock -= VDCLineClock;
    vdcc->DrawBGIndex = 0;

    for (int i = 0; i < 684; i++) {
        vdcc->BGLine[i] = 0x100;
    }

    for (int i = 0; i < vdcc->ScreenSize; i += vdcc->DrawLineWidth) {
        vdcc->DrawBGYLine++;
        if (vdcc->DrawBGYLine == ScreenHeightMAX)
            vdcc->DrawBGYLine = 4;
        if (vdcc->DrawBGYLine < vdcc->VDS + vdcc->VSW) {
            MakeBGColorLineVDC(vdcno);
        } else if (vdcc->DrawBGYLine <= vdcc->VDS + vdcc->VSW + vdcc->VDW) {
            vdcc->DrawBGLine =
                (vdcc->DrawBGYLine == vdcc->VDS + vdcc->VSW ? vdcc->VDCRegister[0x08] : vdcc->DrawBGLine + 1) &
                vdcc->VScreenHeightMask;

            if (!vdcc->VDCBurst) {
                MakeSpriteLine(vdcno);
                MakeBGLine(vdcno);
            } else
                MakeBGColorLineVDC(vdcno);
        } else {
            MakeBGColorLineVDC(vdcno);
        }

        int64_t vline = vdcc->VDS + vdcc->VSW + vdcc->VDW + 1;
        if (vline > 261)
            vline -= 261;
        if (vdcc->DrawBGYLine == vline) {
            vdcc->VDCStatus |= (vdcc->VDCRegister[0x05] & 0x0008) << 2;    // SetVSync INT
            if (vdcc->VRAMtoSATBStartFlag) {
                for (int64_t i = 0, addr = vdcc->VDCRegister[0x13]; i < 256; i++, addr++)
                    vdcc->SATB[i] = vdcc->VRAM[addr];
                vdcc->VRAMtoSATBCount = 256 * Core->vce->VCEBaseClock;
                vdcc->VDCStatus |= 0x40;
                vdcc->VRAMtoSATBStartFlag = (vdcc->VDCRegister[0x0f] & 0x0010) == 0x0010;
            }
        }

        vdcc->RasterCount++;
        if (vdcc->DrawBGYLine == vdcc->VDS + vdcc->VSW - 1)
            vdcc->RasterCount = 64;

        if (vdcc->RasterCount == vdcc->VDCRegister[0x06] && (vdcc->VDCStatus & 0x20) == 0x00)
            vdcc->VDCStatus |= vdcc->VDCRegister[0x05] & 0x0004;

        vdcc->DrawBGIndex += vdcc->DrawLineWidth;
    }
}
void VDC::MakeSpriteLine(int64_t vdcno)
{
    VDCP *vdcc = vdcp[vdcno];

    for (int i = 0; i < vdcc->ScreenWidth; i++) {
        vdcc->SPLine[i].data     = 0x00;
        vdcc->SPLine[i].palette  = 0x000;
        vdcc->SPLine[i].no       = 255;
        vdcc->SPLine[i].priority = 0x00;
    }

    if ((vdcc->VDCRegister[0x05] & 0x0040) == 0x0000)
        return;

    int64_t  dotcount = 0;
    int64_t  line     = vdcc->DrawBGYLine - (vdcc->VDS + vdcc->VSW) + 64;
    int64_t *vram     = vdcc->VRAM;
    int64_t *satb     = vdcc->SATB;

    for (int i = 0, s = 0; i < 64; i++, s += 4) {
        int64_t y         = satb[s] & 0x3ff;
        int64_t attribute = satb[s + 3];
        int64_t height    = ((attribute & 0x3000) >> 8) + 16;
        height            = height > 32 ? 64 : height;
        if (line < y || line > y + height - 1)
            continue;

        int64_t x     = (satb[s + 1] & 0x3ff) - 32;
        int64_t width = ((attribute & 0x0100) >> 4) + 16;

        if (x + width <= 0)
            continue;

        int64_t spy = line - y;
        if ((attribute & 0x8000) == 0x8000)
            spy = height - 1 - spy;

        int64_t  index = ((satb[s + 2] & SPAddressMask[width][height]) << 5) | (((spy & 0x30) << 3) | (spy & 0x0f));
        uint64_t data0;
        uint64_t data1;
        uint64_t data2;
        uint64_t data3;

        if ((attribute & 0x0800) == 0x0000) {
            data0 = ReverseBit16[vram[index]];
            data1 = ReverseBit16[vram[index + 16]];
            data2 = ReverseBit16[vram[index + 32]];
            data3 = ReverseBit16[vram[index + 48]];
            if (width == 32) {
                data0 |= ReverseBit16[vram[index | 0x0040]] << 16;
                data1 |= ReverseBit16[vram[(index | 0x0040) + 16]] << 16;
                data2 |= ReverseBit16[vram[(index | 0x0040) + 32]] << 16;
                data3 |= ReverseBit16[vram[(index | 0x0040) + 48]] << 16;
            }
        } else {
            data0 = vram[index];
            data1 = vram[index + 16];
            data2 = vram[index + 32];
            data3 = vram[index + 48];
            if (width == 32) {
                data0 = (data0 << 16) | vram[index | 0x0040];
                data1 = (data1 << 16) | vram[(index | 0x0040) + 16];
                data2 = (data2 << 16) | vram[(index | 0x0040) + 32];
                data3 = (data3 << 16) | vram[(index | 0x0040) + 48];
            }
        }
        int64_t palette  = ((attribute & 0x000f) << 4) | 0x0100;
        int64_t priority = attribute & 0x0080;
        int64_t j        = 0;

        if (x < 0) {
            j -= x;
            x = 0;
        }

        for (; j < width && x < vdcc->ScreenWidth; j++, x++) {
            if (vdcc->SPLine[x].data == 0x00) {
                uint64_t dot = ((data0 >> j) & 0x0001) | (((data1 >> j) << 1) & 0x0002) |
                               (((data2 >> j) << 2) & 0x0004) | (((data3 >> j) << 3) & 0x0008);

                if (dot != 0x00) {
                    vdcc->SPLine[x].data     = dot;
                    vdcc->SPLine[x].palette  = palette;
                    vdcc->SPLine[x].priority = priority;
                }
            }
            if (vdcc->SPLine[x].no == 255)
                vdcc->SPLine[x].no = i;
            if (i != 0 && vdcc->SPLine[x].no == 0)
                vdcc->VDCStatus |= vdcc->VDCRegister[0x05] & 0x0001;    // SetSpriteCollisionINT
            if (++dotcount == 256) {
                vdcc->VDCStatus |= vdcc->VDCRegister[0x05] & 0x0002;    // SetSpriteOverINT
                if (vdcc->SpriteLimit)
                    return;
            }
        }
    }
}
void VDC::MakeBGLine(int64_t vdcno)
{
    auto vdcc = vdcp[vdcno];

    auto    sp        = vdcc->SPLine;
    auto    bg        = vdcc->BGLine;
    int64_t sw        = vdcc->ScreenWidth;
    int64_t leftblank = ((vdcc->HDS + vdcc->HSW) << 3) + vdcc->DrawBGIndex;

    if ((vdcc->VDCRegister[0x05] & 0x0080) == 0x0080) {
        int64_t WidthMask = vdcc->VScreenWidth - 1;
        int64_t x         = vdcc->VDCRegister[0x07];
        int64_t index_x   = (x >> 3) & WidthMask;
        x                 = (x & 0x07) << 2;
        int64_t y         = vdcc->DrawBGLine;
        int64_t index_y   = ((y >> 3) & (vdcc->VScreenHeight - 1)) * vdcc->VScreenWidth;
        y                 = y & 0x07;
        auto    vram      = vdcc->VRAM;
        int64_t bgx       = 0;
        auto    revbit    = ReverseBit256;

        while (bgx < sw) {
            int64_t  tmp     = vram[index_x + index_y];
            int64_t  address = ((tmp & 0x0fff) << 4) + y;
            int64_t  palette = (tmp & 0xf000) >> 8;
            int64_t  data0   = vram[address];
            int64_t  data1   = vram[address + 8];
            uint64_t data    = revbit[data0 & 0x00ff] | (revbit[(data0 & 0xff00) >> 8] << 1) |
                            (revbit[data1 & 0x00ff] << 2) | (revbit[(data1 & 0xff00) >> 8] << 3);
            for (; x < 32 && bgx < sw; x += 4, bgx++) {
                int64_t dot         = (data >> x) & 0x0f;
                auto    spbgx       = sp[bgx];
                bg[bgx + leftblank] = spbgx.data != 0x00 && (dot == 0x00 || spbgx.priority == 0x0080)
                                          ? spbgx.data | spbgx.palette
                                          : dot | (dot == 0x00 ? 0x00 : palette);
            }
            x       = 0;
            index_x = (index_x + 1) & WidthMask;
        }
    } else {
        for (int i = 0; i < sw; i++)
            bg[i + leftblank] = sp[i].data | sp[i].palette;
    }
}
void VDC::MakeBGColorLineVDC(int64_t vdcno)
{
    for (int i = 0; i < 684; i++) {
        vdcp[vdcno]->BGLine[i] = 0x100;
    }
}
void VDC::VDCProcessDMA(int64_t vdcno)
{
    auto vdcc = vdcp[vdcno];
    if (vdcc->VRAMtoSATBCount > 0) {
        vdcc->VRAMtoSATBCount -= Core->cpu->ProgressClock;

        if (vdcc->VRAMtoSATBCount <= 0)
            vdcc->VDCStatus = (vdcc->VDCStatus & 0xbf) | ((vdcc->VDCRegister[0x0f] & 0x0001) << 3);    // VRAMtoSATB INT
    }
    if (vdcc->VRAMtoVRAMCount > 0) {
        vdcc->VRAMtoVRAMCount -= Core->cpu->ProgressClock;
        if (vdcc->VRAMtoVRAMCount <= 0)
            vdcc->VDCStatus = (vdcc->VDCStatus & 0xbf) | ((vdcc->VDCRegister[0x0f] & 0x0002) << 3);    // VRAMtoVRAM INT
    }
}
void VDC::GetScreenSize(int64_t vdcno)
{
    auto vdcc = vdcp[vdcno];
    auto r    = vdcc->VDCRegister;

    vdcc->VScreenWidth      = VScreenWidthArray[r[0x09] & 0x0030];
    vdcc->VScreenHeight     = (r[0x09] & 0x0040) == 0x0000 ? 32 : 64;
    vdcc->VScreenHeightMask = vdcc->VScreenHeight * 8 - 1;
    vdcc->ScreenWidth       = ((r[0x0b] & 0x007f) + 1) * 8;

    if (vdcc->ScreenWidth > ScreenWidthMAX)
        vdcc->ScreenWidth = ScreenWidthMAX;

    vdcc->HDS        = (r[0x0a] & 0x7f00) >> 8;
    vdcc->HSW        = r[0x0a] & 0x001f;
    vdcc->HDE        = (r[0x0b] & 0x7f00) >> 8;
    vdcc->HDW        = r[0x0b] & 0x007f;
    vdcc->VDS        = (r[0x0c] & 0xff00) >> 8;
    vdcc->VSW        = r[0x0c] & 0x001f;
    vdcc->VDW        = r[0x0d] & 0x01ff;
    vdcc->VCR        = r[0x0e] & 0x00ff;
    vdcc->ScreenSize = ScreenSize[Core->vce->VCEBaseClock];

    if (Core->MainCanvas_width != vdcc->ScreenSize) {
        Core->MainCanvas_width = PutScreenSize[Core->vce->VCEBaseClock];
    }
    vdcc->DrawLineWidth = (vdcc->HDS + vdcc->HSW + vdcc->HDE + vdcc->HDW + 1) << 3;

    if (vdcc->DrawLineWidth <= ScreenSize[Core->cpu->BaseClock5])
        vdcc->DrawLineWidth = ScreenSize[Core->cpu->BaseClock5];
    else if (vdcc->DrawLineWidth <= ScreenSize[Core->cpu->BaseClock7])
        vdcc->DrawLineWidth = ScreenSize[Core->cpu->BaseClock7];
    else
        vdcc->DrawLineWidth = ScreenSize[Core->cpu->BaseClock10];
    vdcc->VDCBurst = (r[0x05] & 0x00c0) == 0x0000 ? true : false;
}
void VDC::SetVDCRegister(int64_t data, int64_t vdcno)
{
    vdcp[vdcno]->VDCRegisterSelect = data & 0x1f;
}
void VDC::SetVDCLow(int64_t data, int64_t vdcno)
{
    auto vdcc = vdcp[vdcno];
    if (vdcc->VDCRegisterSelect == 0x02)
        vdcc->WriteVRAMData = data;
    else
        vdcc->VDCRegister[vdcc->VDCRegisterSelect] = (vdcc->VDCRegister[vdcc->VDCRegisterSelect] & 0xff00) | data;
    if (vdcc->VDCRegisterSelect == 0x01) {
        vdcc->VDCRegister[0x02] = vdcc->VRAM[vdcc->VDCRegister[0x01]];
        return;
    }
    if (vdcc->VDCRegisterSelect == 0x08) {
        vdcc->DrawBGLine = vdcc->VDCRegister[0x08];
        return;
    }
    if (vdcc->VDCRegisterSelect == 0x0f)
        vdcc->VRAMtoSATBStartFlag = (vdcc->VDCRegister[0x0f] & 0x10) == 0x10;
}
void VDC::SetVDCHigh(int64_t data, int64_t vdcno)
{
    auto vdcc = vdcp[vdcno];
    if (vdcc->VDCRegisterSelect == 0x02) {
        vdcc->VRAM[vdcc->VDCRegister[0x00]] = vdcc->WriteVRAMData | (data << 8);
        vdcc->VDCRegister[0x00]             = (vdcc->VDCRegister[0x00] + GetVRAMIncrement(vdcno)) & 0xffff;
        return;
    }
    vdcc->VDCRegister[vdcc->VDCRegisterSelect] = (vdcc->VDCRegister[vdcc->VDCRegisterSelect] & 0x00ff) | (data << 8);
    if (vdcc->VDCRegisterSelect == 0x01) {
        vdcc->VDCRegister[0x02] = vdcc->VRAM[vdcc->VDCRegister[0x01]];
        vdcc->VDCRegister[0x03] = vdcc->VDCRegister[0x02];
        vdcc->VDCRegister[0x01] = (vdcc->VDCRegister[0x01] + GetVRAMIncrement(vdcno)) & 0xffff;
        return;
    }
    if (vdcc->VDCRegisterSelect == 0x08) {
        vdcc->DrawBGLine = vdcc->VDCRegister[0x08];
        return;
    }
    if (vdcc->VDCRegisterSelect == 0x12) {
        int64_t si = (vdcc->VDCRegister[0x0f] & 0x0004) == 0x0000 ? 1 : -1;
        int64_t di = (vdcc->VDCRegister[0x0f] & 0x0008) == 0x0000 ? 1 : -1;
        int64_t s  = vdcc->VDCRegister[0x10];
        int64_t d  = vdcc->VDCRegister[0x11];
        int64_t l  = vdcc->VDCRegister[0x12] + 1;

        vdcc->VRAMtoVRAMCount = l * Core->vce->VCEBaseClock;
        vdcc->VDCStatus |= 0x40;
        auto vram = vdcc->VRAM;

        for (; l > 0; l--) {
            vram[d] = vram[s];
            s       = (s + si) & 0xffff;
            d       = (d + di) & 0xffff;
        }
        return;
    }
    if (vdcc->VDCRegisterSelect == 0x13)
        vdcc->VRAMtoSATBStartFlag = true;
}
int64_t VDC::GetVRAMIncrement(int64_t vdcno)
{
    switch (vdcp[vdcno]->VDCRegister[0x05] & 0x1800) {
        case 0x0000:
            return 1;
        case 0x0800:
            return 32;
        case 0x1000:
            return 64;
        case 0x1800:
            return 128;
    }
    return 0;
}
int64_t VDC::GetVDCStatus(int64_t vdcno)
{
    auto tmp = vdcp[vdcno]->VDCStatus;
    vdcp[vdcno]->VDCStatus &= 0x40;
    return tmp;
}
int64_t VDC::GetVDCLow(int64_t vdcno)
{
    return vdcp[vdcno]->VDCRegister[vdcp[vdcno]->VDCRegisterSelect] & 0x00ff;
}
int64_t VDC::GetVDCHigh(int64_t vdcno)
{
    auto vdcc = vdcp[vdcno];
    if (vdcc->VDCRegisterSelect == 0x02 || vdcc->VDCRegisterSelect == 0x03) {
        int64_t tmp             = (vdcc->VDCRegister[0x02] & 0xff00) >> 8;
        vdcc->VDCRegister[0x02] = vdcc->VRAM[vdcc->VDCRegister[0x01]];
        vdcc->VDCRegister[0x03] = vdcc->VDCRegister[0x02];
        vdcc->VDCRegister[0x01] = (vdcc->VDCRegister[0x01] + GetVRAMIncrement(vdcno)) & 0xffff;
        return tmp;
    }
    return (vdcc->VDCRegister[vdcc->VDCRegisterSelect] & 0xff00) >> 8;
}
