#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "servo.h"
#include "../buzzer/buzzer.h"

//Main function
void main(void)
{
 servo_init_devices();

 servo_1_move(0, 90);
 _delay_ms(1000);
 servo_1_move(90, 0);
 _delay_ms(1000);

 servo_servo_1_free();

 buzzer_on();
 _delay_ms(1000);
 buzzer_off();

 while(1);
}
