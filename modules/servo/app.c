#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "../buzzer/buzzer.h"

void main() {
    servo_init_devices();

    while (1) {
        servo_1_to(20); // Lever
        servo_4_to(160); // Lever

        servo_2_to(0); // Gripper
        servo_3_to(0); // Gripper

        _delay_ms(2000);

        servo_1_to(130); // Lower arm
        servo_4_to(50); // Lower arm

        _delay_ms(2000);

        servo_2_to(85); // Close grip
        servo_3_to(85); // Close grip
        
        _delay_ms(5000);
    }
}
