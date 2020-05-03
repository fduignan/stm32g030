#include "../include/STM32G030.h"
#include <stdint.h>
void delay(uint32_t dly)
{
    while(dly--);
}

int main()
{   
    int i=0;
    
    RCC->IOPENR |= (1 << 0); // enable Port A    
    GPIOA->MODER &= ~(1 << 1); // Make bit 0 an output
    GPIOA->MODER |= (1 << 0);
    
    while(1)
    {
        GPIOA->ODR |= 1;
        delay(300000);
        GPIOA->ODR &= ~1;
        delay(300000);
    }
}

