#include "timer.h"
#include "pce.h"

TIMER::TIMER(PC *_core)
{
    Core           = _core;
    TimerBaseClock = _core->cpu->BaseClock7;
}
void TIMER::TimerInit()
{
    TimerReload    = 0x00;
    TimerFlag      = false;
    TimerCounter   = 0x00;
    TimerPrescaler = 0;
}
int64_t TIMER::ReadTimerCounter()
{
    return TimerCounter;
}
void TIMER::TimerAcknowledge()
{
    Core->irq->INTTIQ = 0x00;
}
void TIMER::WirteTimerReload(int64_t data)
{
    TimerReload = data & 0x7f;
}
void TIMER::WirteTimerControl(int64_t data)
{
    if (!TimerFlag && (data & 0x01) == 0x01) {
        TimerCounter   = TimerReload;
        TimerPrescaler = 0;
    }
    TimerFlag = (data & 0x01) == 0x01 ? true : false;
}
void TIMER::TimerRun()
{
    if (TimerFlag) {
        TimerPrescaler += Core->cpu->ProgressClock;
        while (TimerPrescaler >= 1024 * TimerBaseClock) {
            TimerPrescaler -= 1024 * TimerBaseClock;
            TimerCounter--;
            if (TimerCounter < 0) {
                TimerCounter      = TimerReload;
                Core->irq->INTTIQ = Core->irq->TIQFlag;
            }
        }
    }
}