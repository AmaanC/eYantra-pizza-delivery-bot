#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

// Red   - D1 - Pin 42 PL7
// Green - D2 - Pin 41 PL6
// Blue  - D3 - Pin 44 PD1
// Vcc        - Pin 21

void RgbLedInit() {
    DDRL = DDRL | 0xC0; // Set Port L's 7,6 as output
    DDRD = DDRD | 0x01; // Set Port D's 1 as output
}

void RgbLedOff() {
    PORTL = PORTL | 0xC0; // Reset L 7, 6
    PORTD = PORTD | 0x01;
}

void RgbLedGlow(char color) {
    unsigned char val;
    if(color == 'r') {
            val = 0x7F; // 01111111
            PORTL = PORTL & val;
        }
    if(color == 'g') {
            val = 0xBF; 
            PORTL = PORTL & val;
        }
    if(color == 'b') {
            val = 0x01; // 11111110
            PORTD = PORTD & val;
    }
    _delay_ms(1000);
    RgbLedOff();
    _delay_ms(1000);
}