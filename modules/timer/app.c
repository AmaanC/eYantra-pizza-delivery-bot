#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../buzzer/buzzer.h"

//Main Function
int main(void) {
    InitTimer();
    BuzzerOn();
    RegisterCallback(BuzzerOff, 2);
    _delay_ms(4000);
    FreezeDisplay();
    while(1);
}
