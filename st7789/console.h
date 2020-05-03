#ifndef __console_h
#define __console_h
#include <stdint.h>
#include "timer.h"
#include "display.h"
#include "controller.h"
#include "sound.h"
#define COLOUR(R,G,B) RGBToWord(R,G,B)

class console
{
public:    
    // Graphics functions
    void putPixel(uint16_t x, uint16_t y, uint16_t Colour);
    void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour);
    void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour);    
    void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
    void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour);
    void putImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t * img);
    // Graphics text functions
    void print(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
    void print(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour); 
    // various support functions 
    uint32_t random(uint32_t lower,uint32_t upper);
// public member objects
    timer Timer;
    sound Sound;
    controller Controller;
// various functions that need to be declared but are handled internally (don't call them)    
    console(){}; // Default constructor does nothing.
    void begin(); // initialize console and sub objects (called from init.cpp on boot) 
    display Display; // public but really should be used only internally (isolates the console from hardware dependencies)
private:
    
    int iabs(int x) // simple integer version of abs for use by graphics functions
    {
        if (x < 0)
            return -x;
        else
            return x;
    }
    // sub-functions for line drawing
    void drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);
    void drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour);
    // Pseudo random binary sequence generator for making random numbers
   	unsigned long shift_register;
    void random_seed(); // seed the PRBS generator with a 'random' value
    uint32_t prbs();    // get next prbs output
};
extern console Console;  // The console object is declared/created in console.cpp
#endif
