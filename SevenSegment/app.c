#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SevenSegment.h"

int main(void) {
    seven_init_devices();

    while(1) {
        seven_display_num(12);
        _delay_ms(1);
    }
}
