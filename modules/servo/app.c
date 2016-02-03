#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "../buzzer/buzzer.h"

void main() {
    servo_init_devices();

    servo_1_to(0);
    servo_2_to(0);
    servo_3_to(0);
    servo_4_to(0);
    
    _delay_ms(3000);
    
    while(1) {
        servo_1_to(67);
        servo_2_to(60);
        servo_3_to(60);
        servo_4_to(60);
    }
}
