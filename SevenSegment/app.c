#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../buzzer/buzzer.h"
#include "SevenSegment.h"

//Main
int main(void) {
    seven_init_devices();
    buzzer_init_devices();
    int a = 0x99;
    // This one fails to work
    // int a = seven_convert_to_hex(9);
    int b = 0x49;
    while(1) {
        // seven_display_num(12);
        // _delay_ms(5);
        // buzzer_beep(100);
        PORTD = PORTD & 0x0F;
        PORTD = PORTD | 0x10;
        PORTJ = a;
        _delay_ms(1);
        PORTD = PORTD & 0x0F;
        PORTD = PORTD | 0x20;
        PORTJ = b;
        _delay_ms(1);
        PORTD = PORTD & 0x0F;
        PORTD = PORTD | 0x40;
        PORTJ = a;
        _delay_ms(1);
    }
}
