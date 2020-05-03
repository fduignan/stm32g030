#include "console.h"
#include "font5x7.h"
console Console;
void console::begin()
{
    Controller.begin();
    Timer.begin();
    Display.begin(&Timer);       
    Sound.begin();
    Timer.attach(Sound.ms_callback);  // attach the callback for the sound system to allow contorl playback
// need to attach a timer callback for the sound system so that it can manage playback    
    random_seed();
    
}
// Graphics functions
void console::fillRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour)
{
    Display.fillRectangle(x,y,w,h,Colour);
}
void console::drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t Colour)
{
    drawLine(x,y,x+w,y,Colour);
    drawLine(x,y,x,y+h,Colour);
    drawLine(x+w,y,x+w,y+h,Colour);
    drawLine(x,y+h,x+w,y+h,Colour);
}
void console::drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);
    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles    
    while (x >= y)
    {
        Display.putPixel(x0 + x, y0 + y, Colour);
        Display.putPixel(x0 + y, y0 + x, Colour);
        Display.putPixel(x0 - y, y0 + x, Colour);
        Display.putPixel(x0 - x, y0 + y, Colour);
        Display.putPixel(x0 - x, y0 - y, Colour);
        Display.putPixel(x0 - y, y0 - x, Colour);
        Display.putPixel(x0 + y, y0 - x, Colour);
        Display.putPixel(x0 + x, y0 - y, Colour);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void console::fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t Colour)
{
// Reference : https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
// Similar to drawCircle but fills the circle with lines instead
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    if (radius > x0)
        return; // don't draw even parially off-screen circles
    if (radius > y0)
        return; // don't draw even parially off-screen circles
        
    if ((x0+radius) > SCREEN_WIDTH)
        return; // don't draw even parially off-screen circles
    if ((y0+radius) > SCREEN_HEIGHT)
        return; // don't draw even parially off-screen circles        
    while (x >= y)
    {
        drawLine(x0 - x, y0 + y,x0 + x, y0 + y, Colour);        
        drawLine(x0 - y, y0 + x,x0 + y, y0 + x, Colour);        
        drawLine(x0 - x, y0 - y,x0 + x, y0 - y, Colour);        
        drawLine(x0 - y, y0 - x,x0 + y, y0 - x, Colour);        

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}
void console::drawLineLowSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm    
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0)
  {
    yi = -1;
    dy = -dy;
  }
  int D = 2*dy - dx;
  
  int y = y0;

  for (int x=x0; x <= x1;x++)
  {
    Display.putPixel(x,y,Colour);    
    if (D > 0)
    {
       y = y + yi;
       D = D - 2*dx;
    }
    D = D + 2*dy;
    
  }
}

void console::drawLineHighSlope(uint16_t x0, uint16_t y0, uint16_t x1,uint16_t y1, uint16_t Colour)
{
        // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0)
  {
    xi = -1;
    dx = -dx;
  }  
  int D = 2*dx - dy;
  int x = x0;

  for (int y=y0; y <= y1; y++)
  {
    Display.putPixel(x,y,Colour);
    if (D > 0)
    {
       x = x + xi;
       D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}
void console::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t Colour)
{
    // Reference : https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
    if ( iabs(y1 - y0) < iabs(x1 - x0) )
    {
        if (x0 > x1)
        {
            drawLineLowSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            drawLineLowSlope(x0, y0, x1, y1, Colour);
        }
    }
    else
    {
        if (y0 > y1) 
        {
            drawLineHighSlope(x1, y1, x0, y0, Colour);
        }
        else
        {
            drawLineHighSlope(x0, y0, x1, y1, Colour);
        }
        
    }
}
void console::putPixel(uint16_t x, uint16_t y, uint16_t Colour)
{
    Display.putPixel(x, y, Colour);
}
void console::putImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t * img) 
{
    Display.putImage(x, y, w, h, img);
}

void console::print(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
        // This function draws each character individually.  It uses an array called TextBox as a temporary storage
    // location to hold the dots for the character in question.  It constructs the image of the character and then
    // calls on putImage to place it on the screen
    uint8_t Index = 0;
    uint8_t Row, Col;
    const uint8_t *CharacterCode = 0;    
    uint16_t TextBox[FONT_WIDTH * FONT_HEIGHT];
    for (Index = 0; Index < len; Index++)
    {
        CharacterCode = &Font5x7[FONT_WIDTH * (Text[Index] - 32)];
        Col = 0;
        while (Col < FONT_WIDTH)
        {
            Row = 0;
            while (Row < FONT_HEIGHT)
            {
                if (CharacterCode[Col] & (1 << Row))
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = ForeColour;
                }
                else
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = BackColour;
                }
                Row++;
            }
            Col++;
        }
        Display.putImage(x, y, FONT_WIDTH, FONT_HEIGHT, (const uint16_t *)TextBox);
        x = x + FONT_WIDTH + 2;
    }
}
void console::print(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
     // This function converts the supplied number into a character string and then calls on puText to
    // write it to the display
    char Buffer[5]; // Maximum value = 65535
    Buffer[4] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[3] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[2] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[1] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[0] = Number % 10 + '0';
    print(Buffer, 5, x, y, ForeColour, BackColour);
}
uint32_t console::random(uint32_t lower,uint32_t upper)
{
    return (prbs()%(upper-lower))+lower;
}
uint32_t console::prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here	
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's 
}
void console::random_seed()
{
    // Need to find a source of random data.  This layer of BBG is meant to be 
    // hardware independent so I don't want to break that.  Will have to obtain 
    // a random seed from one of the lower layers of the BBG stack without introducing
    // additional complexity.  Hardware aware object choices: Controller, Display, Timer, Sound
    // An analogue input would be nice so this probably rules out the Display and Timer objects
    // leaving Sound and Controller.  Some variations of BBG may use an analogue joystick so 
    // perhaps a call to readAnalogue(chan_num) might be a good choice?  The channel number
    // can be defined in the controller class (as a static) and mapped appropriately within the
    // Controller object.  Will add up and scale a few reads to change more bits in the shift register
    shift_register = 0;
    while(shift_register == 0)
    {
        for (int i=1;i<17;i++)
            shift_register += Controller.readTemperature()<<i;
    }   
    Controller.stopADC(); // save some power by turning off the ADC and associated circuitry
}
