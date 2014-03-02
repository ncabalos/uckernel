#include <p24Fxxxx.h>

extern void interrupt_handler_timer1(void);
extern void uckernel_tick_handler(void);

void __attribute__((__interrupt__, auto_psv )) _ISR _T1Interrupt (void)
{
    IFS0bits.T1IF = 0;
    uckernel_tick_handler();
    interrupt_handler_timer1();
}
