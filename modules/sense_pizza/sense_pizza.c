#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../custom_delay/custom_delay.h"
#include "../buzzer/buzzer.h"
#include "../sharp_sensor/sharp_sensor.h"
#include "../color_sensor/color_sensor.h"
#include "../lcd/lcd.h"

void InitSenses() {
    SharpInitDevices();
    ColorInitDevices();
    // LcdInit();
}

