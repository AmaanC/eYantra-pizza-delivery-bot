#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "seven_segment.h"

int main(void) {
    SevenInitDevices();

    while(1) {
        SevenDisplayNum(12, 1);
        _delay_ms(1);
        SevenDisplayNum(12, 2);
    }
}
