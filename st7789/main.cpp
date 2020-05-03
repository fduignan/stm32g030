#include "../include/STM32G030.h"
#include "serial.h"
#include <stdint.h>
#include "console.h"
#include "spi.h"
#include "timer.h"
serial Serial;
void delay(uint32_t dly)
{
    while(dly--);
}

int main()
{   
    int Count=0;
    
    initSPI();
    Serial.begin();
    Console.begin();

    RCC->IOPENR |= (1 << 0); // enable Port A    
    GPIOA->MODER &= ~(1 << 1); // Make bit 0 an output
    GPIOA->MODER |= (1 << 0);
    
    /* uncomment this to route clock to pin 18 for measurement 
    GPIOA->MODER |= (1  << 17);
    GPIOA->MODER &= ~(1  << 16);
    RCC->CFGR |= (1 << 30)  | (1 << 29) | (1 << 28); // mco scaling = /128
    RCC->CFGR |=  (1 << 24); // mco select
    */

    while(1)
    {
        
        if (Serial.available())
        {
            Serial.print(Serial.egetc());
            GPIOA->ODR |= 1;
            delay(30000);
            GPIOA->ODR &= ~1;            
        }
        
        if (1) {
        // Draw some random rectangles
        for (Count = 0; Count < 200; Count++)
        {
            Console.drawRectangle(Console.random(0,240),Console.random(0,240),Console.random(0,120),Console.random(0,120),Console.random(0,0xffff));
        //    Console.Timer.sleep(50);                        
        }
        Console.fillRectangle(0,0,240,240,0);
        for (Count = 0; Count < 200; Count++)
        {
            Console.fillRectangle(Console.random(0,240),Console.random(0,240),Console.random(0,120),Console.random(0,120),Console.random(0,0xffff));
         //   Console.Timer.sleep(50);                        
        }
        Console.fillRectangle(0,0,240,240,0);
        for (Count = 0; Count < 200; Count++)
        {
            Console.drawCircle(Console.random(0,240),Console.random(0,240),Console.random(0,120),Console.random(0,0xffff));
         //   Console.Timer.sleep(50);                        
        }
        Console.fillRectangle(0,0,240,240,0);
        for (Count = 0; Count < 50; Count++)
        {
            Console.fillCircle(Console.random(0,240),Console.random(0,240),Console.random(0,120),Console.random(0,0xffff));
         //   Console.Timer.sleep(50);                        
        }
        Console.fillRectangle(0,0,240,240,0);
        }
    }
}

