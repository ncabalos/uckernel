#include <p24Fxxxx.h>
#include <stdbool.h>

extern void interrupt_handler_timer1(void);
extern void uckernel_tick_handler(void);

void __attribute__((__interrupt__, auto_psv )) _ISR _OscillatorFail (void)
{
    INTCON1bits.OSCFAIL = 0;

    while(true);
}
void __attribute__((__interrupt__, auto_psv )) _ISR _AddressError (void)
{
    INTCON1bits.ADDRERR = 0;

    while(true);
}
void __attribute__((__interrupt__, auto_psv )) _ISR _StackError (void)
{
    INTCON1bits.STKERR = 0;

    while(true);
}
void __attribute__((__interrupt__, auto_psv )) _ISR _MathError (void)
{
    INTCON1bits.MATHERR = 0;

    while(true);
}

void __attribute__((__interrupt__, auto_psv )) _ISR _T1Interrupt (void)
{
    IFS0bits.T1IF = 0;
    uckernel_tick_handler();
    interrupt_handler_timer1();
}
