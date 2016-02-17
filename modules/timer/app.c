#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../buzzer/buzzer.h"

void Timer1sMagic() {
    BuzzerOn();
    _delay_ms(1000);
    BuzzerOff();
    _delay_ms(1000);
}

//Main Function
int main(void) {
    TimerInitDevices();
    while(1);
}
