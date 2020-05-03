#ifndef __sound_h
#define __sound_h
#include <stdint.h>
#define STARTUP_JINGLE_LENGTH 13

const uint16_t DefaultStartupJingleTones[STARTUP_JINGLE_LENGTH]={3*1047,3*1319,3*1175,3*1397,3*1319,3*1568,3*1397,3*1047,3*1047,3*1568,3*1047,3*1568,3*1047};
const uint16_t DefaultStartupJingleTimes[STARTUP_JINGLE_LENGTH]={200,200,200,200,200,200,200,200,200,200,200,200,200};
class sound
{
public:
    sound(){};
    void begin();
    void playTone(uint16_t Frequency, uint16_t ms);
    static void stopTone();
    int  SoundPlaying();
    void playMelody(const uint16_t *Tones,const uint16_t *Times,int Len);
    static void ms_callback(void);
private:
    volatile uint32_t tone_time;          
};
#endif
