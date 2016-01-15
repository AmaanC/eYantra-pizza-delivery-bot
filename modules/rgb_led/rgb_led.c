#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

// Red   - D1 - Pin 19
// Green - D2 - Pin 38
// Blue  - D3 - Pin 37
void rgb_led_init() {
    DDRD = DDRD | 0x0E; // Set Port D's 1, 2, 3 as output
}

void rgb_led_off() {
    PORTD = PORTD & 0xF1; // Reset D1, 2, 3
}

void rgb_led_glow(char color) {
    int val = 0xF1; // Off by default
    switch(color) {
        case 'r':
            value = 0x02;
            break;
        case 'g':
            value = 0x04;
            break;
        case 'b':
            value = 0x08;
            break;
    }
    rgb_led_off();
    PORTD = PORTD | val;
}