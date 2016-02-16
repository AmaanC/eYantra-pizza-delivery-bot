#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "../buzzer/buzzer.h"

void main() {
    ServoInitDevices();
    const int UP_LEVER_1 = 20;
    const int UP_LEVER_2 = 160;
    const int DOWN_LEVER_1 = 130;
    const int DOWN_LEVER_2 = 50;

    const int OPEN_GRIPPER_1 = 60;
    const int OPEN_GRIPPER_2 = 125;
    const int CLOSE_GRIPPER_1 = 120;
    const int CLOSE_GRIPPER_2 = 95;

    while (1) {
        Servo1To(UP_LEVER_1); // Lever up
        Servo4To(UP_LEVER_2); // Lever up

        Servo2To(OPEN_GRIPPER_1); // Gripper open
        Servo3To(OPEN_GRIPPER_2); // Gripper open

        _delay_ms(2000);

        Servo1To(DOWN_LEVER_1); // Lever down
        Servo4To(DOWN_LEVER_2); // Lever down

        _delay_ms(2000);

        Servo2To(CLOSE_GRIPPER_1); // Close grip
        Servo3To(CLOSE_GRIPPER_2); // Close grip

        _delay_ms(1000);

        Servo1To(UP_LEVER_1); // Lever up
        Servo4To(UP_LEVER_2); // Lever up

        _delay_ms(1000);

        Servo1To(DOWN_LEVER_1); // Lever down
        Servo4To(DOWN_LEVER_2); // Lever down
        
        _delay_ms(5000);
    }
}
