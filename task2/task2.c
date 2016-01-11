#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../timer/timer.h"
#include "../SevenSegment/SevenSegment.h"

int count = 0;

void timer_1s_magic() {
    count++;
    if (count >= 30) {
        count = 30;
    }
}

int main() {
    timer_init_devices();
    seven_init_devices();
    while(1) {
       seven_display_num(count);
       _delay_ms(1);
    }
}
