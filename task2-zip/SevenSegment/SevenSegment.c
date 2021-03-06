#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SevenSegment.h"

void seven_segment_pin_config() {
    // Port D upper nibble for CA connections
    // Port J for a,b,c,d, e,f,g,DEC as per manual
    // Both things above are in decreasing order
    // i.e.
    // Port J = Pin 7, 6, 5...0

    DDRD = DDRD | 0xF0; // set upper nibble of port D to output
    DDRJ = DDRJ | 0xFF; // set all bits to output
}

void seven_port_init() {
    seven_segment_pin_config();
}

void seven_init_devices() {
    cli(); //Clears the global interrupt
    seven_port_init();  //Initializes all the ports
    sei();   // Enables the global interrupt
}

int seven_convert_to_hex(int num) {
    int ret = 0x00;
    int values[10];
    values[0] = 0x03;
    values[1] = 0x9F;
    values[2] = 0x25;
    values[3] = 0x0D;
    values[4] = 0x99;
    values[5] = 0x49;
    values[6] = 0x41;
    values[7] = 0x1F;
    values[8] = 0x01;
    values[9] = 0x09;

    if (num >= 0 && num <= 9) {
        ret = values[num];
    }
    return ret;
}

void seven_display_num(int num) {
    int digit;
    int MIN_SELECT_VALUE = 0x10;
    int MAX_SELECT_VALUE = 0x40;

    int select_value = MIN_SELECT_VALUE; // Initially 0x10. Refer to pins.txt for other values (they just double)
    PORTD = PORTD & 0x0F; // Reset upper nibble to 0
    PORTD = PORTD | select_value;

    while (select_value <= MAX_SELECT_VALUE) {
        digit = num % 10;
        PORTJ = seven_convert_to_hex(digit);
        _delay_ms(5);
        select_value *= 2;
        if (select_value <= MAX_SELECT_VALUE) {
            PORTD = PORTD & 0x0F; // Reset upper nibble to 0
            PORTD = PORTD | select_value; // Set upper nibble value to select apt CA
        }
        num /= 10;
    }
}
