#include "sound.h"
#include <stdint.h>
#include "../include/STM32G030.h"
// Will use Pin 10 of the STM32F030 TSSOP-20 which is PA4/TIM14_CH1 (AF4)
static sound *pSound; // A hack to to allow registration of callbacks while decoupling the sound and timer classes.

void sound::begin()
{
    pSound = this;
     // Output is a piezo speaker on A4 which is TIM14
    // Turn on Timer 14
    RCC->APBENR2 |= (1 << 15);  
    // Ensure that Port A is enabled
    RCC->IOPENR |= (1 << 0);
    // Set Timer 14 to default values
    TIM14->CR1 = 0; 
    // Enable PWM mode on channel 1
    TIM14->CCMR1_Output = (1 << 6)+(1 << 5); 
    // Connect up Timer 14 Channel 1 output
    TIM14->CCER |= (1<<0);
}
void sound::playTone(uint16_t Frequency, uint16_t ms)
{
 // Configure PA4 as Timer3 output
    GPIOA->MODER &= ~(1 << 8); // Select alternate function
    GPIOA->MODER |= (1 << 9);
    //
    GPIOA->AFRL &= ~( (1<<19) + (1 << 17) + (1 << 16) ); // Timer is AF4
    GPIOA->AFRL |= (1 << 18);
    // Will assume a 48MHz input frequency
    // The auto-reload register has a maximum value of 65536.  
    // This should map to the lowest frequency we would be interested in.
    // 48MHz/65536 = 732Hz.  This is too high for a 'lowest' frequency so 
    // some pre-scaling of the input frequency is required.
    TIM14->CR1 &= ~(1 << 0); // disable the timer    
    TIM14->PSC = 48000000UL/65536UL;  // prescale so that the lowest frequency is 1Hz.
    TIM14->ARR = (48000000UL/(uint32_t)(TIM14->PSC))/((uint32_t)Frequency);
    TIM14->CCR1 = TIM14->ARR/2; // 50 % Duty cycle
    TIM14->CNT = 0;
    TIM14->CR1 |= (1 << 0); // enable the timer        
    pSound->tone_time = ms;    
}
void sound::stopTone()
{
    TIM3->CR1 &= ~(1 << 0); // disable the timer
    // Force the output low
     // Configure PA4 as a high impedance input (seems to save power)
    GPIOA->MODER &= ~((1 << 8)+(1 << 9)); // Select high impedance input function	
}
int  sound::SoundPlaying()
{
    if (tone_time > 0)
        return 1;
    else        
        return 0;
}
void sound::playMelody(const uint16_t *Tones,const uint16_t *Times,int Len)
{
    int Index;
    for (Index = 0; Index < Len; Index++)
    {
        while(SoundPlaying()); // wait for previous note to complete        
        playTone(Tones[Index],Times[Index]);               
    }
}
void sound::ms_callback()
{
    // This callback should run every millisecond.  It is attached to the Timer object in console.cpp:begin
    if (pSound->tone_time)
    {
        pSound->tone_time--;
        if (pSound->tone_time == 0)
        {
            pSound->stopTone();
        }
    }
}
