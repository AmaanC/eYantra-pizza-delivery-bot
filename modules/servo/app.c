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
        ServoControl(1, UP_LEVER_1); // Lever up
        _delay_ms(1000);
        ServoControl(4, UP_LEVER_2); // Lever up
        _delay_ms(1000);

        ServoControl(2, OPEN_GRIPPER_2); // Gripper open
        _delay_ms(1000);
        ServoControl(3, OPEN_GRIPPER_1); // Gripper open

        _delay_ms(1000);

        ServoControl(1, DOWN_LEVER_1); // Lever down
        _delay_ms(1000);
        ServoControl(4, DOWN_LEVER_2); // Lever down

        _delay_ms(1000);

        ServoControl(3, CLOSE_GRIPPER_1); // Close grip
        
        _delay_ms(3000);

        ServoControl(2, CLOSE_GRIPPER_2); // Close grip

        _delay_ms(3000);

        ServoControl(1, UP_LEVER_1); // Lever up

        _delay_ms(1000);

        ServoControl(4, UP_LEVER_2); // Lever up

        _delay_ms(5000);

        ServoControl(1, DOWN_LEVER_1); // Lever down
        _delay_ms(1000);
        ServoControl(4, DOWN_LEVER_2); // Lever down
        
        _delay_ms(1000);
        ServoControl(2, OPEN_GRIPPER_2); // Gripper open
        _delay_ms(1000);
        ServoControl(3, OPEN_GRIPPER_1); // Gripper open

        _delay_ms(5000);
    }
}
