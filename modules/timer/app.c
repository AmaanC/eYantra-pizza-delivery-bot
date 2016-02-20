#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../buzzer/buzzer.h"

//Main Function
int main(void) {
    TimerInitDevices();
    while(1);
}
