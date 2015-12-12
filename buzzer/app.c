#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "buzzer.h"

//Main Function
int main(void)
{
    init_devices();
    while(1)
    {
        buzzer_on();
        _delay_ms(1000);        //delay
        buzzer_off();
        _delay_ms(3000);        //delay
    }
}