#ifndef __DISPLAY_H
#define __DISPLAY_H
#include <stdint.h>
#include "timer.h"
// Assumption: Top left of display is 0,0
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240
// It seems the ST7789 has space in memory for 320 rows.  The LCD panel 
// attached to this model only has 240 rows which means that 80 rows can be
// drawn off-screen which is a great help to scroller games possibly
#define VIRTUAL_SCREEN_HEIGHT 320
// Define a macro to allow easy definition of colours
// Format of colour value: <BGND 1 bit><Red 5 bits><Green 5 bits><Blue 5 bits>
//#define RGBToWord( R,  G,  B)  (  ((G&0xf8) << (11-3)) | ((R&0xfc) << (5-2)) | ((B&0xf8)>>3) )
#define RGBToWord( R,  G,  B)  (  ((R&0xf8) << (11-3)) | ((B&0xfc) << (5-2)) | ((G&0xf8)>>3) )
class display
{
public:
    display(){};
    void begin(timer *t);
    void putPixel(uint16_t x, uint16_t y, uint16_t colour);
    void putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour);
    void openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); 
    void writeData16(uint16_t Data);        
    void readDisplayID(uint8_t *data);
    void scroll(int n);
private:
    void RSLow();
    void RSHigh();
    void resetDisplay();
    void writeCommand(uint8_t Cmd);
    void writeData8(uint8_t Data);
   
    void RSTLow();
    void RSTHigh();            
};
#endif
