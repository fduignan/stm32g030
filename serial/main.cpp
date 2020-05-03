#include "../include/STM32G030.h"
#include "serial.h"
#include <stdint.h>
serial Serial;
void delay(uint32_t dly)
{
    while(dly--);
}
void initClock();
int main()
{   
    int i=0;
    Serial.begin();
    RCC->IOPENR |= (1 << 0); // enable Port A    
    GPIOA->MODER &= ~(1 << 1); // Make bit 0 an output
    GPIOA->MODER |= (1 << 0);
    while(1)
    {
        
        if (Serial.available())
        {
            Serial.print(Serial.egetc());
            GPIOA->ODR |= 1;
            delay(30000);
            GPIOA->ODR &= ~1;            
        }
        else
        {
            
        }
                
    }
}

