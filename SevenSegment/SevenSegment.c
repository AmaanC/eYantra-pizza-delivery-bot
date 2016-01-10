//#define __OPTIMIZE__ -O0
//#define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SevenSegment.h"
#include "../buzzer/buzzer.h"

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

int seven_convert_to_hex(int num) {
    int ret = 0x00;
    // int values[] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09};
    // ret = values[num];
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
        case 3:
            ret = 0x0D;
            break;
        case 4:
            ret = 0x99;
            break;
        case 5:
            ret = 0x49;
            break;
        case 6:
            ret = 0x41;
            break;
        case 7:
            ret = 0x1F;
            break;
        case 8:
            ret = 0x01;
            break;
        case 9:
            ret = 0x09;
            break;
        default:
            ret = 0x00;
            break;
    }
    return ret;
}

void seven_display_num(int num) {
    int digit;
    int our_num = 14;
    int MIN_SELECT_VALUE = 0x10;
    int MAX_SELECT_VALUE = 0x20;

    int select_value = MIN_SELECT_VALUE; // Initially 0x10. Refer to pins.txt for other values (they just double)
    PORTD = PORTD & 0x0F; // Reset upper nibble to 0
    PORTD = PORTD | select_value;

    while (select_value <= MAX_SELECT_VALUE) {
        digit = our_num % 10;
        if (digit == 6) {
            buzzer_beep(100); // The buzzer does beep here for when "our_num" becomes 16
            if (seven_convert_to_hex(digit) == 0x41) {
                buzzer_beep(200); // The buzzer does beep
            }
        }
        if (digit == 4) {
            buzzer_beep(300); // The buzzer does beep here for when "our_num" becomes 16
            if (seven_convert_to_hex(digit) == 0x99) {
                buzzer_beep(500); // The buzzer does beep
            }
        }
        PORTJ = seven_convert_to_hex(digit);
        _delay_ms(500);
        select_value *= 2;
        if (select_value <= MAX_SELECT_VALUE) {
            PORTD = PORTD & 0x0F; // Reset upper nibble to 0
            PORTD = PORTD | select_value; // Set upper nibble value to select apt CA
        }
        // If I do any of the following (anywhere, before or after), the 7-segment display shows *nothing* on the screen.
        our_num = 16; // 16 or any other constant
        // our_num /= 10;
        // Using num or our_num makes no difference. Neither work.
    }
}
