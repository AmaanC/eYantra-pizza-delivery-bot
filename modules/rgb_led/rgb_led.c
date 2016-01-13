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

void rgb_led_red() {
    rgb_led_off();
    PORTD = PORTD | 0x02;
}

void rgb_led_green() {
    rgb_led_off();
    PORTD = PORTD | 0x04;
}

void rgb_led_blue() {
    rgb_led_off();
    PORTD = PORTD | 0x08;
}