#include "serial.h"
#include <stdint.h>
#include "../include/STM32G030.h"

static serial *pSerial;  // need a pointer to the hopefully only instance of Serial for the interrupt service routine
void serial::begin()
{
    int BaudRateDivisor;
    int BaudRate = 9600;
    pSerial = this;
	disable_interrupts();
	RXBuffer.head = RXBuffer.tail = RXBuffer.count = 0;
	TXBuffer.head = TXBuffer.tail = TXBuffer.count = 0;	
// Turn on the clock for GPIOA (usart 2 uses it)
    RCC->IOPENR |= (1 << 0); // enable Port A    
// Turn on the clock for the USART2 peripheral	
	RCC->APBENR1 |= (1 << 17);

	
// Configure the I/O pins.  Will use PA3 as RX and PA2 as TX
    GPIOA->MODER |= ((1 << 7) + (1 << 5));
    GPIOA->MODER &= ~((1 << 6) + (1 << 4));
    
    // select AF1 for PA2 and PA3
    GPIOA->AFRL &= ~(0xff00);
    GPIOA->AFRL |= ((1 << 12) + (1 << 8));
    
	
	BaudRateDivisor = 64000000; // assuming 64MHz clock
	BaudRateDivisor = BaudRateDivisor / (long) BaudRate;

// For details of what follows refer to RM0454 Rev2, USART registers.

// De-assert reset of USART2 
	RCC->APBRSTR1 &= ~(1 << 17);
// Configure the USART
// disable USART first to allow setting of other control bits
// This also disables parity checking and enables 16 times oversampling

	USART2->CR1 = 0; 
 
// Don't want anything from CR2
	USART2->CR2 = 0;

// Don't want anything from CR3
	USART2->CR3 = 0;

// Set the baud rate    
    
    /* Divisor = 32000000/(9600)=3333.33333               
    */
	USART2->BRR = BaudRateDivisor;
// Clear any pending interrupts
    USART2->ICR = 0xffffffff;
// Turn on Transmitter, Receiver, Transmit and Receive interrupts.
	USART2->CR1 = ((1 << 2)  | (1 << 3) |  (1 << 5) | (1 << 6) ); 
// Enable the USART    
	USART2->CR1 |= (1 << 0);
// Enable USART2 interrupts in NVIC	
	NVIC->ISER |= (1 << 28); // enable interrupt # 28 (USART2) 
    enable_interrupts();
}
void serial::eputc(char c)
{
	if ( (USART2->CR1 & (1 << 3))==0)
	{ // transmitter was idle, turn it on and force out first character
        USART2->CR1 |= (1 << 3);        
        USART2->TDR = c; //getBuf(TXBuffer);		
	} 
    else
    {   // interrupt transmission is underway so add byte to the queue
        putBuf(TXBuffer,c);
    }
}
char serial::egetc()
{
    // return next character in buffer (0 if empty)
    return getBuf(RXBuffer);
}
void serial::print(int Value)
{
    // Convert Value to a string equivalent and call the 
    // print(string) version of this function
    
    char Buffer[12]; // ints can be +/- 2 billion (approx) so buffer needs to be this big
    int index=10;    // Start filling buffer from the right
    Buffer[11]=0;    // ensure the buffer is terminated with a null
    if (Value < 0)  
    {
        Buffer[0]='-';  // insert a leading minus sign
        Value = -Value; // make Value positive
    }
    else
    {
        Buffer[0]='+';  // insert a leading plus sign
    }
    while(index > 0)  // go through all of the characters in the buffer
    {
        Buffer[index]=(Value % 10)+'0'; // convert the number to ASCII
        Value = Value / 10; // move on to the next digit
        index--;  
    }
    print(Buffer); // call string version of print
}
void serial::print(char c)
{    
    eputc(c); // write out the character        
}
void serial::print(const char *String)
{
    while(*String) // keep going until a null is encountered
    {
        eputc(*String); // write out the next character
        String++;       // move the pointer along
    }
}
// Maybe the circular buffer code belongs in a more general purpose module
void serial::putBuf(SerialBuffer &sbuf, char c)
{
    if (sbuf.count < SBUFSIZE)
    {
        disable_interrupts();
        sbuf.count++;
        sbuf.buffer[sbuf.head] = c;
        sbuf.head=(sbuf.head+1) % SBUFSIZE;
        enable_interrupts();
    }
}
char serial::getBuf(SerialBuffer &sbuf)
{
    char c=0;
    if (sbuf.count >0 )
    {
        disable_interrupts();
        sbuf.count--;
        c=sbuf.buffer[sbuf.tail];
        sbuf.tail=(sbuf.tail+1) % SBUFSIZE;
        enable_interrupts();
    }
    return c;
}
int serial::available() 
{
    return RXBuffer.count;
}
// This is an interrupt handler for serial communications.  
void USART2_Handler(void)
{
// check which interrupt happened.       
    if (USART2->ISR & (1 << 7)) // is it a TXE interrupt?
    {
        USART2->ICR |= ((1 << 5) + (1 << 6));
        if (pSerial->TXBuffer.count > 0) // if there is another byte to send
        {
            USART2->TDR=pSerial->getBuf(pSerial->TXBuffer);
        }
        else
        {
            // No more data, disable the transmitter 
            USART2->CR1 &= ~(1 << 3);            
        }
    }
	if (USART2->ISR & (1 << 5)) // is it an RXNE interrupt?
    {
        // read the RX hardware buffer and put 
        // it into the circular buffer
        pSerial->putBuf(pSerial->RXBuffer,USART2->RDR);         	
        USART2->ICR |= (1 << 5);
    }

}

