#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../buzzer/buzzer.h"

void timer_1s_magic() {
    buzzer_on();
    _delay_ms(1000);
    buzzer_off();
    _delay_ms(1000);
}

//Main Function
int main(void) {
    timer_init_devices();
    while(1);
}
