#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "color_sensor.h"
#include "../buzzer/buzzer.h"

int main() {
    char color;
    color_init_devices();
    while(1) {
        // Alerts us that it's reading the colour now
        buzzer_beep(100);

        color = color_get();
        switch(color) {
            // 2 short
            case 'r':
                buzzer_beep(100);
                buzzer_beep(100);
                break;
            // 2 long
            case 'g':
                buzzer_beep(300);
                buzzer_beep(300);
                break;
            // 1 long, 1 short
            case 'b':
                buzzer_beep(300);
                buzzer_beep(100);
                break;
            // 3 short for error
            default:
                buzzer_beep(100);
                buzzer_beep(100);
                buzzer_beep(100);
                break;
        }

        // Read color again in 5s
        _delay_ms(5000);
    }
    return 0;
}