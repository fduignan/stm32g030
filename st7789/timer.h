#ifndef __timer_h
#define __timer_h
/*
The Timer class provides time-keeping functions and supports up to 8 callback functions for use in games or system services.
The Timer class bases it's timekeeping on the SysTick timer which has an interrupt frequency of 1000Hz. 
*/

#include <stdint.h>

#define MAX_TIMER_CALLBACKS 8
typedef void (*fptr)(void);    
class timer {
public:
    timer(){};
    void begin();  // This function sets up the SysTick timer interrupt and clears out the callback array
    int attach(fptr cb); // register a new callback function : returns 0 if ok, -1 if limit has been reached
    int detach(fptr cb); // remove a callback function (the array is search for the function with the specified address)
    void sleep(uint32_t dly); // Wait for the specified number of ms to pass.  Do not call on this in interrupt context.
                              // The sleep function also attempts to save power by putting the CPU to sleep
private:
    friend void Systick_Handler(void); // Interrupt service routine for the systick interrupt
   
    fptr CallbackArray[MAX_TIMER_CALLBACKS]; // The array of callback functions that can be called.
};

#endif
