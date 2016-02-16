#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

// Red   - D1 - Pin 19
// Green - D2 - Pin 38
// Blue  - D3 - Pin 37
// Vcc        - Pin 21

void RgbLedInit() {
    DDRD = DDRD | 0x0E; // Set Port D's 1, 2, 3 as output
}

void RgbLedOff() {
    PORTD = PORTD & 0xF1; // Reset D1, 2, 3
}

void RgbLedGlow(char color) {
    int val = 0xF1; // Off by default
    switch(color) {
        case 'r':
            val = 0xFD; // 1101
            break;
        case 'g':
            val = 0xFB; // 1011
            break;
        case 'b':
            val = 0xF7; // 0111
            break;
    }
    RgbLedOff();
    PORTD = PORTD & val;
}