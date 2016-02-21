#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../custom_delay/custom_delay.h"
#include "../buzzer/buzzer.h"
#include "../sharp_sensor/SharpSensor.h"
#include "../color_sensor/color_sensor.h"
#include "../lcd/lcd.h"

int main() {
    InitSenses();
    char pizza_size, pizza_color;
    while(1) {
        pizza_color = color_get();
        pizza_size = SharpGetBlockType();

        LcdPrintf("color: %c", pizza_color);
        _delay_ms(1000);
        LcdPrintf("size: %c", pizza_size);
        _delay_ms(1000);
    }
}
    