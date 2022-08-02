#include "irq.h"
#include "pce.h"

IRQ::IRQ(PC *_core)
{
    Core = _core;
}
int64_t IRQ::GetIntStatus()
{
    return ~IntDisableRegister & GetIntReqest();
}
int64_t IRQ::GetIntDisable()
{
    return IntDisableRegister;
}
void IRQ::SetIntDisable(int64_t data)
{
    IntDisableRegister = data;
    Core->timer->TimerAcknowledge();
}
int64_t IRQ::GetIntReqest()
{
    int64_t val =
        ((((Core->vdc->vdcp[0]->VDCStatus | Core->vdc->vdcp[1]->VDCStatus) & 0x3f) != 0x00 ? IRQ1Flag : 0x00) |
         INTIRQ2 | INTTIQ);

    return val;
}
void IRQ::SetIntReqest(int64_t data)
{
    Core->timer->TimerAcknowledge();
}
void IRQ::Init()
{
    Reset();
}
void IRQ::Reset()
{
    TIQFlag            = 0x04;
    IRQ1Flag           = 0x02;
    IRQ2Flag           = 0x01;
    INTTIQ             = 0x00;
    INTIRQ2            = 0x00;
    IntDisableRegister = 0x00;
}
