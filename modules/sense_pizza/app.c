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
    LcdInit();
    char pizza_color, pizza_size;
    while(1) {
        LcdPrintf("Place Pizza!");
        _delay_ms(1500);
        pizza_size = SharpGetBlockType();
        pizza_color = GetPizzaColor();
        LcdPrintf("color:%c, size:%c", pizza_color, pizza_size);
    }
}
    