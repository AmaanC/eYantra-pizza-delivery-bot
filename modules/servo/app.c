#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "../buzzer/buzzer.h"

void main() {
    servo_init_devices();

    servo_1_to(20); // Lever
    servo_2_to(0); // Gripper
    _delay_ms(2000);
    servo_1_to(115); // Lower arm
    _delay_ms(2000);
    servo_2_to(85); // Close grip
    
    servo_3_to(0);
    servo_4_to(0);
    
    while(1) {
        servo_3_to(60);
        servo_4_to(60);
    }
}
