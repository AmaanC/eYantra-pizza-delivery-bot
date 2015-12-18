//#define __OPTIMIZE__ -O0
//#define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SevenSegment.h"

void seven_segment_pin_config(void)
{
    // Port D upper nibble for CA connections
    // Port J for a,b,c,d, e,f,g,DEC as per manual
    // Both things above are in decreasing order
    // i.e.
    // Port J = Pin 7, 6, 5...0

    DDRD = DDRD | 0xF0; // set upper nibble of port D to output
    DDRJ = DDRJ | 0xFF; // set all bits to output
}

void seven_port_init(void)
{
    seven_segment_pin_config();
}

void seven_init_devices(void)
{
    cli(); //Clears the global interrupt
    seven_port_init();  //Initializes all the ports
    sei();   // Enables the global interrupt
}

int seven_convert_to_hex(unsigned int num) {
    int ret = 0x00;
    switch(num) {
        case 0:
            ret = 0x03;
            break;
        case 1:
            ret = 0x9F;
            break;
        case 2:
            ret = 0x25;
            break;
        case 6:
            ret = 0x41;
            break;
        case 9:
            ret = 0x09;
            break;
    }
    return ret;
}

void seven_display_num(unsigned int num) {
    int digit;
    int MIN_SELECT_VALUE = 0x10;
    int MAX_SELECT_VALUE = 0x40;

    int select_value = MIN_SELECT_VALUE; // Initially 0x10. Refer to pins.txt for other values (they just double)
    PORTD = PORTD & 0x0F; // Reset upper nibble to 0
	PORTD = PORTD | select_value;

    while (select_value <= MAX_SELECT_VALUE) {
        digit = num % 10;
        PORTJ = seven_convert_to_hex(digit);
        _delay_ms(1);
        select_value *= 2;
		if (select_value <= MAX_SELECT_VALUE) {
        	PORTD = PORTD & 0x0F; // Reset upper nibble to 0
        	PORTD = PORTD | select_value; // Set upper nibble value to select apt CA
		}
        num /= 10;
    }
}
