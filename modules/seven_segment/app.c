#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "seven_segment.h"
#include "../lcd/lcd.h"

int main(void) {
    SevenInitDevices();
    LcdInit();

    while(1) {
        SevenDisplayNum(12, 1);
        _delay_ms(7);
        SevenDisplayNum(12, 2);
        _delay_ms(7);
        SevenDisplayNum(12, 3);
        _delay_ms(7);
    }
}
