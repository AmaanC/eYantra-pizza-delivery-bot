#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "../buzzer/buzzer.h"

void main() {
    servo_init_devices();

    servo_servo_1(90);
    _delay_ms(1000);
    servo_servo_1(0);
    while(1) {
        // _delay_ms(5000);
        // servo_1_move(90, 0);
        // _delay_ms(5000);
    }
}
