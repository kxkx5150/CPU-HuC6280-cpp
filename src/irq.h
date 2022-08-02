#ifndef IRQ_H
#define IRQ_H
#include <cstdint>

class PC;
class IRQ {
  public:
    PC      *Core               = nullptr;
    int64_t TIQFlag            = 0x04;
    int64_t IRQ1Flag           = 0x02;
    int64_t IRQ2Flag           = 0x01;
    int64_t INTTIQ             = 0x00;
    int64_t INTIRQ2            = 0x00;
    int64_t IntDisableRegister = 0x00;

    IRQ(PC *_core);

    int64_t GetIntStatus();
    int64_t GetIntDisable();
    void     SetIntDisable(int64_t data);
    int64_t GetIntReqest();
    void     SetIntReqest(int64_t data);
    void     Init();
    void     Reset();
};
#endif