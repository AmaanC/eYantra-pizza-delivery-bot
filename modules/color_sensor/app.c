#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "color_sensor.h"
#include "../buzzer/buzzer.h"
#include "../custom_delay/custom_delay.h"

int main() {
    char color;
    ColorInitDevices();
    while(1) {
        // Alerts us that it's reading the colour now
        BuzzerBeep(100);

        color = color_get();
        switch(color) {
            // 2 short
            case 'r':
                BuzzerBeep(100);
                BuzzerBeep(100);
                break;
            // 2 long
            case 'g':
                BuzzerBeep(300);
                BuzzerBeep(300);
                break;
            // 1 long, 1 short
            case 'b':
                BuzzerBeep(300);
                BuzzerBeep(100);
                break;
            // 3 short for error
            default:
                BuzzerBeep(100);
                BuzzerBeep(100);
                BuzzerBeep(100);
                break;
        }

        // Read color again in 5s
        _delay_ms(5000);
    }
    return 0;
}