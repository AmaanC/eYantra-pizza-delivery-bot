#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../buzzer/buzzer.h"
#include "SevenSegment.h"



//Main
int main(void) {
    seven_init_devices();
    buzzer_init_devices();
    // int num = 123;
    // int digit;
    // int select_value = 0x10;
    while(1) {
        seven_display_num(12);
        _delay_ms(1);

        // digit = num % 10;
        // PORTD = PORTD & 0x0F;
        // select_value = select_value & 0xF0; // Clear the lower nibble to be safe
        // PORTD = PORTD | select_value; // Set upper nibble to select value
        // // PORTJ = values[i % 10];
        // PORTJ = seven_convert_to_hex(digit);
        // // if (i > 2) {
        // //     i = 0;
        // // }
        // select_value *= 2;
        // if (select_value > 0x40) {
        //     select_value = 0x10;
        // }
        // num /= 10;
        // _delay_ms(1);
    }
}
