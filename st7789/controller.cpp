#include <stdint.h>
#include "../include/STM32G030.h"
#include "controller.h" 
/* 
 * Button configuratoin (Version 0.1)
 * PF0, PF1, PA0, PA1, PA2: Mapped to bits 0 to 4 in return of getButtonState respectively
 */
void controller::begin()
{
    RCC->APBENR2 |= (1 << 20); // enable ADC clock  
    ADC->CFGR2 |= (1 << 31); // 
    ADC->CFGR2 &= ~(1 << 30); // ADC Clock = HSI16/4
    ADC->CR = 0;
    ADC->CHSELR = (1 << 12);
    ADC->CR = (1 << 28); // Turn on the voltage regulator
    ADC->CCR |= (1 << 22) + (1 << 23);  // enable voltage reference and temperature sensor
    volatile int dly=0;
    while (dly < 1000) dly++; // wait for regulator to stabilize
    ADC->CR |= (1 << 31); // start calibration
    while ( ADC->CR & (1 << 31) ); // wait for calibration to complete
    ADC->SMPR = 0x01; // short S+H time
    ADC->CR = 1; // enable the ADC
    
/*  Disabled for now as it gets in the way of using the UART for debugging   
    // Turn on port F   
    RCC->IOPENR |= (1 << 5);
    // Turn on port A 
    RCC->IOPENR |= (1 << 0);
    // Configure pins as inputs
    GPIOF->MODER &= ~(0x0f); // PF0 bits 0 and 1 are inputs
    GPIOA->MODER &= ~(0x3f); // PA0 bits 0,1,2 are inputs    
    // Enable pull-ups on button inputs
    GPIOF->PUPDR &= ~(0x0f);
    GPIOF->PUPDR |= (1 << 2) + (1 << 0);
    GPIOA->PUPDR &= ~(0x3f);
    GPIOA->PUPDR |= (1 << 4) + (1 << 2) + (1 << 0);
*/
}
void controller::stopADC()
{
    ADC->CR = 0;
    ADC->CCR = 0;
    RCC->APBENR2 &= ~(1 << 20); // disable ADC clock  
}
uint16_t controller::getButtonState()
{
    uint16_t ReturnValue = 0;
    if (( GPIOF->IDR & (1 << 0) )==0)
    {
        ReturnValue |= Right;
    }
    if (( GPIOF->IDR & (1 << 1) )==0)
    {
        ReturnValue |= Down;
    }
    if (( GPIOA->IDR & (1 << 0) )==0)
    {
        ReturnValue |= Fire;
    }
    if (( GPIOA->IDR & (1 << 1) )==0)
    {
        ReturnValue |= Up;
    }
    if (( GPIOA->IDR & (1 << 2) )==0)
    {
        ReturnValue |= Left;
    }
    return ReturnValue;
}
uint16_t controller::readTemperature()
{
    ADC->CHSELR = (1 << 12);
    ADC->CR |= 1+(1 << 2); // start a conversion
    while((ADC->CR & (1 << 2))==0); // wait for EOC
    return ADC->DR; 
}
