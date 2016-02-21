#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rgb_led.h"

int main() {
    RgbLedInit();

    while(1) {
        RgbLedGlow('r');
        RgbLedGlow('b');
        RgbLedGlow('g');
    }
    return 0;
}