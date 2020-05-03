#include <stdint.h>


class controller
{
public:
    controller(){};
    void begin();
    uint16_t getButtonState();    
    uint16_t readTemperature();
    void stopADC();
    // Some class-level button constants
    static const int Left = 1; 
    static const int Right = 2; 
    static const int Fire = 4;
    static const int Up = 8;
    static const int Down = 16;
};
