#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

// Red   - L7 - Pin 17
// Green - L6 - Pin 18 
// Blue  - L1 - Pin 27 
// Vcc        - Pin 21

void RgbLedInit() {
    DDRL = DDRL | 0xC2; // Set Port L's 7,6,1 as output 
}

void RgbLedOff() {
    PORTL = PORTL | 0xC2; // Reset L 7, 6, 1
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
        val = 0xFD; // 11111101
        PORTL = PORTL & val;
    }
    _delay_ms(1000);

    
}