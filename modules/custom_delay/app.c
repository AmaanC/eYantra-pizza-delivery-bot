#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "custom_delay.h"
#include "../buzzer/buzzer.h"

int main() {
    while (1) {
        BuzzerOn();
        CustomDelay(100);
        BuzzerOff();
        CustomDelay(300);
    }
}