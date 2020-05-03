#include "timer.h"
#include "../include/STM32G030.h"
static timer *pTimer; // need a pointer to the hopefully only instance of Timer for the interrupt service routine
static volatile uint32_t milliseconds;
void timer::begin()
{
    pTimer = this;
    // initialize all callback addresses to zero 
    for (int i=0;i < MAX_TIMER_CALLBACKS;i++)
    {
        CallbackArray[i]=0;
    }
    // The systick timer is driven by a 48MHz clock
    // Divide this down to achieve a 1ms timebase
    // Divisor = 48MHz/1000Hz
    // Reload value = 48000-1
    // enable systick and its interrupts
    STK->CSR |= (7); 
    STK->RVR = 48000-1; // generate 1 millisecond time base
    STK->CVR = 5;
    enable_interrupts();

}
int timer::attach(fptr cb)
{
    for (int i=0;i < MAX_TIMER_CALLBACKS;i++)
    {
        if (CallbackArray[i]==0) 
        {
            // found a free slot
            CallbackArray[i]=cb;
            return 0;
        }
    }
    return -1; // no room left
}
int timer::detach(fptr cb)
{
    for (int i=0;i < MAX_TIMER_CALLBACKS;i++)
    {
        if (CallbackArray[i]==cb) 
        {
            // found the slot so empty it
            CallbackArray[i]=0;
            return 0;
        }
    }   
    return -1; // didn't find that one
        
}
void timer::sleep(uint32_t dly)
{  // Do not call on this in interrupt context
   
    if (dly)
    {   
        milliseconds = 0;        
        while(milliseconds<dly)
        {                                    
            asm(" wfi "); // may as well sleep while waiting for millisecond to elapse (saves power)
        }
    }  
}
void Systick_Handler(void)
{
    milliseconds++;
    for (int i=0;i < MAX_TIMER_CALLBACKS;i++)
    {
        if (pTimer->CallbackArray[i]!=0) 
        {
            // found a used slot so call it
            pTimer->CallbackArray[i]();
        }
    }
}
