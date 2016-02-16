#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "buzzer.h"
#include "../custom_delay/custom_delay.h"

//Main Function
int main(void) {
    BuzzerInitDevices();
    while(1)
    {
        BuzzerOn();
        _delay_ms(1000);        //delay
        BuzzerOff();
        _delay_ms(3000);        //delay
    }
}