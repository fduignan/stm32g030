#include <stdint.h>
#include "display.h"
#include "spi.h"
#include "../include/STM32G030.h"

void display::begin(timer *t)
{
    
    // Turn on Port A 
    RCC->IOPENR |= (1 << 0);     
    // Reset pin is on PA6
    GPIOA->MODER |= (1 << 12);
    GPIOA->MODER &= ~(1 << 13);
        
    // D/C pin is on PB1
    // Turn on Port B
    RCC->IOPENR |= (1 << 1);     
    GPIOB->MODER |= (1 << 2);
    GPIOB->MODER &= ~(1 << 3);
    
    
    RSTHigh(); // Drive reset high
    t->sleep(25); // wait   
    RSTLow(); // Drive reset low
    t->sleep(25); // wait   
    RSTHigh(); // Drive reset high
    t->sleep(25); // wait    
    
    
    writeCommand(0x1);  // software reset
    t->sleep(150); // wait   
    
    writeCommand(0x11);  //exit SLEEP mode
    t->sleep(25); // wait   
    
    writeCommand(0x3A); // Set colour mode        
    writeData8(0x55); // 16bits / pixel @ 64k colors 5-6-5 format 
    t->sleep(25); // wait   
    
    writeCommand(0x36);
    writeData8(0x08);  // RGB Format
    t->sleep(25); // wait   
    
    
    writeCommand(0x51); // maximum brightness
    t->sleep(25); // wait   
    
    writeCommand(0x21);    // display inversion off (datasheet is incorrect on this point)
    writeCommand(0x13);    // partial mode off                 
    writeCommand(0x29);    // display on
    
    t->sleep(25); // wait   
    writeCommand(0x2c);   // put display in to write mode
    fillRectangle(0,0,SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT, 0x00);  // black out the screen
}
void display::putPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    openAperture(x, y, x + 1, y + 1);
    writeData16(colour); 
}
void display::putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image)
{
    uint16_t Colour;
    uint16_t LowerY = height+y;
    if ((LowerY) <= VIRTUAL_SCREEN_HEIGHT) 
    {
        openAperture(x, y, x + width - 1, y + height - 1);
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                Colour = *(Image++);
                writeData16(Colour);
            }
        }
    }
    else
    {
        // Drawing a box beyond the extents of the virtual screen.  
        // Need to wrap this around to the start of the screen.
        uint16_t LowerHeight = (VIRTUAL_SCREEN_HEIGHT-y);
        uint16_t UpperHeight = height - LowerHeight;
        openAperture(x, y, x + width - 1, VIRTUAL_SCREEN_HEIGHT-1);
        for (int j = 0; j < LowerHeight; j++)
        {
            for (int i = 0; i < width; i++)
            {
                Colour = *(Image++);
                writeData16(Colour);
            }
        }
        openAperture(x, 0,x + width - 1, UpperHeight);
        for (int j = 0; j < UpperHeight; j++)
        {
            for (int i = 0; i < width; i++)
            {
                Colour = *(Image++);
                writeData16(Colour);
            }
        } 
    }
}
void display::fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour)
{
    
    uint32_t pixelcount = height * width;
    uint16_t LowerY = height+y;
    if ((LowerY) <= VIRTUAL_SCREEN_HEIGHT) 
    {
        openAperture(x, y, x + width - 1, y + height - 1);
        RSHigh();
        while(pixelcount--)
            transferSPI16(Colour);
    }
    else
    {
        // Drawing a box beyond the extents of the virtual screen.  
        // Need to wrap this around to the start of the screen.
        uint16_t LowerHeight = (VIRTUAL_SCREEN_HEIGHT-y);
        uint16_t UpperHeight = height - LowerHeight;
        openAperture(x, y, x + width - 1, VIRTUAL_SCREEN_HEIGHT-1);
        RSHigh();
        pixelcount = LowerHeight * width;
        while(pixelcount--)
            transferSPI16(Colour);
      
        openAperture(x, 0,x + width - 1, UpperHeight);
        RSHigh();
        pixelcount = UpperHeight * width;
        while(pixelcount--)
                transferSPI16(Colour);
        
    }
}
void display::openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // open up an area for drawing on the display    
    RSLow(); // Set X limits
    transferSPI8(0x2A);
    RSHigh();
    transferSPI8(x1>>8);
    transferSPI8(x1&0xff);        
    transferSPI8(x2>>8);
    transferSPI8(x2&0xff);
    RSLow(); // Set Y limits
    transferSPI8(0x2B);
    RSHigh();    
    transferSPI8(y1>>8);
    transferSPI8(y1&0xff);        
    transferSPI8(y2>>8);
    transferSPI8(y2&0xff);    
        
    RSLow(); // put display in to data write mode
    transferSPI8(0x2c);

}
void display::RSLow()
{ 
// Using Set/Reset register here as this needs to be as fast as possible   
    GPIOB->BSRR = ((1 << 1) << 16); // drive D/C pin low
}
void display::RSHigh()
{ 
// Using Set/Reset register here as this needs to be as fast as possible     
    GPIOB->BSRR = ((1 << 1)); // drive D/C pin high
}
void display::RSTLow()
{
    GPIOA->ODR &= ~(1 << 6); // Drive reset low
}
void display::RSTHigh()
{
    GPIOA->ODR |= (1 << 6); // Drive reset high
}
void display::writeCommand(uint8_t Cmd)
{
    RSLow();
    transferSPI8(Cmd);
}
void display::writeData8(uint8_t Data)
{
    RSHigh();
    transferSPI8(Data);
}
void display::writeData16(uint16_t Data)
{
    RSHigh();
    transferSPI16(Data);
}
void display::readDisplayID(uint8_t *data)
{
    // This is not working - I'm not sure that it is even possible to
    // read the display over SPI
    volatile int x=0;
    //writeCommand(0x09); // Read command
    RSLow();
    uint8_t *preg=(uint8_t*)&SPI1->DR;
     *preg = 0x04;
    //while( (SPI1->SR & (1 << 7)) == 0);
    while(x++ < 100);
    RSHigh();
  //  The following seems to set the SPI interface off sending lots of clocks.
    SPI1->CR1 &= ~(1 << 14); // read mode

    // First read is a dummy read
    data[0] = *preg;// transferSPI8(0);
    while( (SPI1->SR & 1) == 0);
        SPI1->CR1 &= ~(1 << 14); // read mode
    data[0] = *preg;// transferSPI8(0);
            SPI1->CR1 &= ~(1 << 14); // read mode
    while( (SPI1->SR & 1) == 0);
    data[1] = *preg;// transferSPI8(1);        
        SPI1->CR1 &= ~(1 << 14); // read mode
    while( (SPI1->SR & 1) == 0);
    data[2] = *preg;// transferSPI8(2);    
                SPI1->CR1 &= ~(1 << 14); // read mode
    while( (SPI1->SR & 1) == 0);
    data[3] = *preg;// transferSPI8(2);
    
    while( (SPI1->SR & 1) == 0);
    data[3] = *preg;// transferSPI8(2);
    SPI1->CR1 |= (1 << 14); // write mode
   // writeCommand(0x2c); // put display in to data write mode

} 
void display::scroll(int n)
{
    // commands 0x33 and 0x37
    writeCommand(0x33);    
    RSHigh();
    transferSPI16(0);    
    transferSPI16(0x140);    
    transferSPI16(0);        
    writeCommand(0x37);
    n = n % VIRTUAL_SCREEN_HEIGHT;
    RSHigh();    
    transferSPI8(n >> 8);
    transferSPI8(n & 0xff);
    
}
