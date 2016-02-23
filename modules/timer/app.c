#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../buzzer/buzzer.h"
#include "../lcd/lcd.h"

//Main Function
int main(void) {
    InitTimer();
    // SevenInitDevices();
    LcdInit();
    // BuzzerOn();
    RegisterCallback(BuzzerOff, 1000);
    // _delay_ms(4000);
    // BuzzerBeep(1000);
    // FreezeDisplay();
    // _delay_ms(10000);
    // FreezeDisplay();
    LcdPrintf("Hello");
    _delay_ms(1000);
    while(1) {
        LcdPrintf("Time: %d", GetCurrentTime());
    };
    return 0;
}
