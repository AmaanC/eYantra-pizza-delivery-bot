#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "../buzzer/buzzer.h"

void main() {
    servo_init_devices();

    servo_1_to(90);
    servo_2_to(90);
    servo_3_to(90);
    servo_4_to(90);
    
    _delay_ms(1000);
    
    servo_1_to(0);
    servo_2_to(0);
    servo_3_to(0);
    servo_4_to(0);
    while(1) {

    }
}
