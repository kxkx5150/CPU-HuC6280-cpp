#ifndef IOMEM_H
#define IOMEM_H

#include <cstdint>

class PC;
class Mem {
  public:
    PC     *Core      = nullptr;
    int64_t MPRSelect = 0;
    int64_t RAMMask   = 0x1fff;
    bool    BRAMUse   = false;

    int64_t MPR[8];
    int64_t RAM[0x8000];
    int64_t BRAM[0x2000];

  public:
    Mem(PC *_core);

    int64_t Get(int64_t address);
    void    Set(int64_t address, int64_t data);
    int64_t Get16(int64_t address);
    void    StorageReset();
    void    Init();
};

#endif