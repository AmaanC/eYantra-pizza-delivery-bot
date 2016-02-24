#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../seven_segment/seven_segment.h"
#include "../buzzer/buzzer.h"
#include "../lcd/lcd.h"

//Main Function
int main(void) {
    InitTimer();
    LcdInit();
    BuzzerOn();
    RegisterCallback(BuzzerOff, 1000);
    _delay_ms(4000);
    // BuzzerBeep(1000);
    FreezeDisplay();
    _delay_ms(1000);
    FreezeDisplay();
    // // LcdPrintf("Hello");
    // // _delay_ms(2000);
    // BuzzerOn();
    // RegisterCallback(BuzzerOff, 1000);
    while(1) {
        LcdPrintf("Time: %d %d", GetCurrentTime(), GetCallbackTime());
    };
    return 0;
}
