#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../buzzer/buzzer.h"
#include "SevenSegment.h"

//Main
int main(void) {
    seven_init_devices();
    buzzer_init_devices();
    while(1) {
        seven_display_num(12);
        _delay_ms(5);
        // buzzer_beep(100);
    }
}
