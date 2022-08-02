#ifndef RCPU_H
#define RCPU_H

#include <cstdint>
#include <string>
#include <vector>
#include "mem.h"

class PC;
class CPU {
  public:
    Mem *mem = nullptr;

    int64_t A             = 0;
    int64_t X             = 0;
    int64_t Y             = 0;
    int64_t PC            = 0;
    int64_t S             = 0;
    int64_t P             = 0;
    int64_t NFlag         = 0x80;
    int64_t VFlag         = 0x40;
    int64_t TFlag         = 0x20;
    int64_t BFlag         = 0x10;
    int64_t DFlag         = 0x08;
    int64_t IFlag         = 0x04;
    int64_t ZFlag         = 0x02;
    int64_t CFlag         = 0x01;
    int64_t ProgressClock = 0;
    int64_t CPUBaseClock  = 0;
    int64_t BaseClock1    = 12;
    int64_t BaseClock3    = 6;
    int64_t BaseClock5    = 4;
    int64_t BaseClock7    = 3;
    int64_t BaseClock10   = 2;
    int64_t TransferSrc   = 0;
    int64_t TransferDist  = 0;
    int64_t TransferLen   = 0;
    int64_t TransferAlt   = 0;
    int64_t LastInt       = 0;

    int64_t NZCacheTable[0x100];

    int64_t OpCycles[300] = {
        8, 7, 3, 4, 6, 4, 6, 7, 3, 2, 2, 2, 7, 5, 7, 6, 2, 7, 7, 4, 6, 4, 6, 7, 2, 5, 2, 2, 7, 5, 7, 6, 7, 7, 3, 4, 4,
        4, 6, 7, 3, 2, 2, 2, 5, 5, 7, 6, 2, 7, 7, 2, 4, 4, 6, 7, 2, 5, 2, 2, 5, 5, 7, 6, 7, 7, 3, 4, 8, 4, 6, 7, 3, 2,
        2, 2, 4, 5, 7, 6, 2, 7, 7, 5, 2, 4, 6, 7, 2, 5, 3, 2, 2, 5, 7, 6, 7, 7, 2, 2, 4, 4, 6, 7, 3, 2, 2, 2, 7, 5, 7,
        6, 2, 7, 7, 0, 4, 4, 6, 7, 2, 5, 3, 2, 7, 5, 7, 6, 4, 7, 2, 7, 4, 4, 4, 7, 2, 2, 2, 2, 5, 5, 5, 6, 2, 7, 7, 8,
        4, 4, 4, 7, 2, 5, 2, 2, 5, 5, 5, 6, 2, 7, 2, 7, 4, 4, 4, 7, 2, 2, 2, 2, 5, 5, 5, 6, 2, 7, 7, 8, 4, 4, 4, 7, 2,
        5, 2, 2, 5, 5, 5, 6, 2, 7, 2, 0, 4, 4, 6, 7, 2, 2, 2, 2, 5, 5, 7, 6, 2, 7, 7, 0, 2, 4, 6, 7, 2, 5, 3, 2, 2, 5,
        7, 6, 2, 7, 2, 0, 4, 4, 6, 7, 2, 2, 2, 2, 5, 5, 7, 6, 2, 7, 7, 0, 2, 4, 6, 7, 2, 5, 3, 2, 2, 5, 7, 6,
    };
    int64_t OpBytes[300] = {
        0, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 0, 0, 2, 2, 2, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 0, 0, 2, 1, 2, 2,
        2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 0, 0, 2, 2, 1, 2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 0, 0, 2, 1, 2, 0, 2, 2, 2, 1, 2,
        1, 1, 0, 3, 3, 0, 0, 2, 2, 2, 1, 2, 2, 2, 1, 3, 1, 1, 1, 3, 3, 0, 0, 2, 1, 1, 2, 2, 2, 2, 1, 2, 1, 1, 0, 3, 3,
        0, 0, 2, 2, 0, 2, 2, 2, 2, 1, 3, 1, 1, 0, 3, 3, 0, 0, 2, 1, 3, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 0, 0, 2, 2, 4,
        2, 2, 2, 2, 1, 3, 1, 1, 3, 3, 3, 0, 2, 2, 2, 3, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 0, 0, 2, 2, 4, 2, 2, 2, 2, 1,
        3, 1, 1, 3, 3, 3, 0, 2, 2, 1, 0, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 0, 0, 2, 2, 0, 1, 2, 2, 2, 1, 3, 1, 1, 1, 3,
        3, 0, 2, 2, 1, 0, 2, 2, 2, 2, 1, 2, 1, 1, 3, 3, 3, 0, 0, 2, 2, 0, 1, 2, 2, 2, 1, 3, 1, 1, 1, 3, 3, 0,
    };

    int64_t                 counts          = 0;
    bool                     logcheck        = true;
    std::string              filename        = "log0.txt";
    int                      filecheck_start = 0;
    int                      filecheck_end   = 100000;
    int                      fileoffset      = 0;
    bool                     stepinfo        = false;
    std::vector<std::string> lines;

  public:
    CPU(Mem *_mem);

    void CPUInit();

    void     CPUReset();
    void     CPURun();
    void     OpExec(int64_t op);
    void     Adder(int64_t address, int64_t neg);
    void     ADC(int64_t address);
    void     SBC(int64_t address);
    void     AND(int64_t address);
    void     EOR(int64_t address);
    void     ORA(int64_t address);
    int64_t ASL(int64_t data);
    int64_t LSR(int64_t data);
    int64_t ROL(int64_t data);
    int64_t ROR(int64_t data);
    void     BBRi(int64_t bit);
    void     BBSi(int64_t bit);
    void     Branch(bool status, int64_t adr);
    void     Compare(int64_t data0, int64_t data1);
    int64_t Decrement(int64_t data);
    int64_t Increment(int64_t data);
    void     Push(int64_t data);
    int64_t Pull();
    void     RMBi(int64_t bit);
    void     SMBi(int64_t bit);
    void     BIT(int64_t address);
    void     TST(int64_t address0, int64_t address1);
    void     TRB(int64_t address);
    void     TSB(int64_t address);
    int64_t Load(int64_t address);
    void     Store(int64_t address, int64_t data);
    int64_t ZP();
    int64_t ZP_X();
    int64_t ZP_Y();
    int64_t IND();
    int64_t IND_X();
    int64_t IND_Y();
    int64_t ABS();
    int64_t ABS_X();
    int64_t ABS_Y();
    int64_t ABS_IND();
    int64_t ABS_X_IND();
    void     SetNZFlag(int64_t data);
    int64_t PCH();
    int64_t PCL();
    void     toPCH(int64_t data);
    void     toPCL(int64_t data);

    void Dump(int64_t op);
    int  file_read();
};

#endif /* RCPU_H */
