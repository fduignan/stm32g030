#include "../include/STM32G030.h"
#include "adc.h"
#include <stdint.h>
// This code reads the uncalibrated device internal temperature  
void adc::begin()
{   
    RCC->APBENR2 |= (1 << 20); // enable ADC clock  
    ADC->CFGR2 |= (1 << 31); // 
    ADC->CFGR2 &= ~(1 << 30); // ADC Clock = HSI16/4
    ADC->CR = 0;
    ADC->CHSELR = (1 << 12);
    ADC->CR = (1 << 28); // Turn on the voltage regulator
    ADC->CCR |= (1 << 22) + (1 << 23);  // enable voltage reference and temperature sensor
    volatile int dly=0;
    while (dly < 1000) dly++; // wait for regulator to stabilize 20 microseconds
    ADC->CR |= (1 << 31); // start calibration
    while ( ADC->CR & (1 << 31) ); // wait for calibration to complete
    ADC->SMPR = 0x01; // short S+H time
    ADC->CR = 1; // enable the ADC
}
int adc::read()
{
        
    ADC->CHSELR = (1 << 12);
    ADC->CR |= 1+(1 << 2); // start a conversion
    while((ADC->CR & (1 << 2))==0); // wait for EOC
    return ADC->DR;
}
