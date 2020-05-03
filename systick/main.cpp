#include "../include/STM32G030.h"
#include <stdint.h>
void delay(uint32_t dly)
{
    while(dly--);
}

void Systick_Handler()
{ 
    static uint32_t milliseconds = 0;
    
    milliseconds++;
    if (milliseconds == 1000)
    {
        milliseconds = 0;
        GPIOA->ODR ^= 1; // Toggle LED to allow timing to be measured   
    
    }
       
    
}
void initSystick()
{
    
    STK->CSR |= ( 7 ); // enable systick, source = cpu clock, enable interrupt
// SysTick clock source = 64MHz.  Divide this down to create 1 millisecond period
    STK->RVR = 64000-1;   
    STK->CVR = 10; // don't want long wait for counter to count down from initial high unknown value
}
int main()
{   
    int i=0;
    
    RCC->IOPENR |= (1 << 0); // enable Port A    
    GPIOA->MODER &= ~(1 << 1); // Make bit 0 an output
    GPIOA->MODER |= (1 << 0);
    
    initSystick();
    enable_interrupts();
    while(1)
    {
        
   
    }
}

