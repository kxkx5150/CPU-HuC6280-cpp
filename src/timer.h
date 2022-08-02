#ifndef TIMER_H
#define TIMER_H
#include <cstdint>

class PC;
class TIMER {
  public:
    PC *Core = nullptr;

    int64_t TimerReload    = 0;
    bool     TimerFlag      = false;
    int64_t  TimerCounter   = 0;
    int64_t TimerPrescaler = 0;
    int64_t TimerBaseClock = 0;

    TIMER(PC *_core);
    void     TimerInit();
    int64_t ReadTimerCounter();
    void     TimerAcknowledge();
    void     WirteTimerReload(int64_t data);
    void     WirteTimerControl(int64_t data);
    void     TimerRun();
};
#endif
