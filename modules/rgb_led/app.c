#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

int main() {
    RgbLedInit();
    while(1) {
        RgbLedGlow('r');
        RgbLedOff();
        _delay_ms(10);
        RgbLedGlow('g');
        RgbLedOff();
        _delay_ms(10);
        RgbLedGlow('b');
        RgbLedOff();
        _delay_ms(10);
    }
    return 0;
}
