// This example prints the uncalibrated internal temperature to the UART
#include "../include/STM32G030.h"
#include "serial.h"
#include "adc.h"
#include <stdint.h>

adc Analog;
serial Serial;
void delay(volatile uint32_t dly)
{
    while(dly--);
}
int main()
{   
    int i=0;
    Analog.begin();
    Serial.begin();
    RCC->IOPENR |= (1 << 0); // enable Port A    
    GPIOA->MODER &= ~(1 << 1); // Make bit 0 an output
    GPIOA->MODER |= (1 << 0);
    while(1)
    {
        
        GPIOA->ODR |= 1;
        Serial.print("Value = ");
        Serial.print(Analog.read());
        Serial.print("\r\n");
        GPIOA->ODR &= ~1;            
        delay(30000);                       
    }
}

