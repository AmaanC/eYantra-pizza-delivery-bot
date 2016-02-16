#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

int main() {
    while(1) {
        RgbLedGlow('r');
        _delay_ms(1000);
        
        RgbLedGlow('g');
        _delay_ms(1000);
        
        RgbLedGlow('b');
        _delay_ms(1000);
    }
    return 0;
}