#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

int main() {
    while(1) {
        rgb_led_red();
        _delay_ms(1000);
        
        rgb_led_green();
        _delay_ms(1000);
        
        rgb_led_blue();
        _delay_ms(1000);
    }
    return 0;
}