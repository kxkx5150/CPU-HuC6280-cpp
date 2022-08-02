#ifndef VDC_H
#define VDC_H
#include <cstdint>

class PC;

struct SPLineData
{
    int64_t data     = 0;
    int64_t no       = 255;
    int64_t priority = 0;
    int64_t palette  = 0;
};

class VDCP {
  public:
    bool VDCBurst            = false;
    bool SpriteLimit         = false;
    bool VRAMtoSATBStartFlag = false;

    int64_t VDCStatus         = 0x00;
    int64_t VDCRegisterSelect = 0x00;
    int64_t WriteVRAMData     = 0x0000;
    int64_t VRAMtoSATBCount   = 0;
    int64_t VRAMtoVRAMCount   = 0;
    int64_t RasterCount       = 64;
    int64_t VDCProgressClock  = 0;
    int64_t DrawBGYLine       = 0;
    int64_t DrawBGLine        = 0;
    int64_t VScreenWidth      = 0;
    int64_t VScreenHeight     = 0;
    int64_t VScreenHeightMask = 0;
    int64_t ScreenWidth       = 0;
    int64_t ScreenSize        = 0;
    int64_t DrawLineWidth     = 0;
    int64_t DrawBGIndex       = 0;
    int64_t HDS               = 0;
    int64_t HSW               = 0;
    int64_t HDE               = 0;
    int64_t HDW               = 0;
    int64_t VDS               = 0;
    int64_t VSW               = 0;
    int64_t VDW               = 0;
    int64_t VCR               = 0;

    SPLineData SPLine[684];
    int64_t    BGLine[684];
    int64_t    VDCRegister[20];
    int64_t    VRAM[0x10000];
    int64_t    SATB[256];

    VDCP()
    {
        for (int i = 0; i < 684; i++) {
            BGLine[i] = 0;
        }
        for (int i = 0; i < 20; i++) {
            VDCRegister[i] = 0;
        }
        for (int i = 0; i < 0x10000; i++) {
            VRAM[i] = 0;
        }
        for (int i = 0; i < 256; i++) {
            SATB[i] = 0;
        }
    }
};
class VDC {
  public:
    PC *Core = nullptr;

    uint32_t imgdata[684 * 262]{};
    int64_t  imgidx = 0;

    int64_t VDCPutLineProgressClock = 0;
    int64_t VDCPutLine              = 0;
    int64_t VDCLineClock            = 1368;

    int64_t ScreenHeightMAX = 262;
    int64_t ScreenWidthMAX  = 684;

    int64_t counts = 0;

    VDCP *vdcp[2];

    int64_t ScreenSize[13];
    int64_t PutScreenSize[13];
    int64_t VScreenWidthArray[0x31];

    int64_t ReverseBit[0x100];
    int64_t ReverseBit256[0x100];
    int64_t ReverseBit16[0x10000];
    int64_t SPAddressMask[33][65];

  public:
    VDC(PC *_core);
    void    VDCInit();
    void    VDCRun();
    void    VDCProcess(int64_t vdcno);
    void    MakeSpriteLine(int64_t vdcno);
    void    MakeBGLine(int64_t vdcno);
    void    MakeBGColorLineVDC(int64_t vdcno);
    void    VDCProcessDMA(int64_t vdcno);
    void    GetScreenSize(int64_t vdcno);
    void    SetVDCRegister(int64_t data, int64_t vdcno);
    void    SetVDCLow(int64_t data, int64_t vdcno);
    void    SetVDCHigh(int64_t data, int64_t vdcno);
    int64_t GetVRAMIncrement(int64_t vdcno);
    int64_t GetVDCStatus(int64_t vdcno);
    int64_t GetVDCLow(int64_t vdcno);
    int64_t GetVDCHigh(int64_t vdcno);
};
#endif
