#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "color_sensor.h"
#include "../custom_delay/custom_delay.h"


volatile unsigned long int color_pulse = 0;
const int color_data_time = 100;


void ColorPinConfig() {
    DDRH = DDRH | 0xF0; // Set upper nibble of Port H as output
    DDRJ = DDRJ | 0x01; // Set PJ0 as output
    DDRD = DDRD & 0xFE; // Set pin 0 of Port D as input (interrupt)
}

void ColorInterruptInit() {
    // Enable INT0 (at D0)
    cli();
    EICRA = EICRA | 0x02; // INT0 is set to trigger with falling edge
    EIMSK = EIMSK | 0x01; // Enable Interrupt INT0 for color sensor
    sei();
}

ISR(INT0_vect) {
    color_pulse++;
}

int ColorCollectPulses() {
    color_pulse = 0;
    _delay_ms(color_data_time);
    return color_pulse;
}

int ColorGetRed() {
    PORTH = PORTH & 0xBF; //set S2 low
    PORTH = PORTH & 0x7F; //set S3 low
    return ColorCollectPulses();
}

int ColorGetGreen() {
    PORTH = PORTH | 0x40; //set S2 High
    PORTH = PORTH | 0x80; //set S3 High
    return ColorCollectPulses();
}

int ColorGetBlue() {
    PORTH = PORTH & 0xBF; //set S2 low
    PORTH = PORTH | 0x80; //set S3 High
    return ColorCollectPulses();
}

void ColorSensorScaling() {
    PORTJ = PORTJ | 0x01; //set S0 high
    PORTH = PORTH | 0x20; //set S1 high
}

void ColorInitDevices() {
    ColorPinConfig();
    ColorInterruptInit();
    ColorSensorScaling();
}

char color_get() {
    // It will return the following characters:
    // u for unknown, by default (if something went wrong)
    // r for red
    // g for green
    // b for blue

    // It'll ideally take 300ms for this function to run, since we collect 100ms of data for each filter
    
    char ret = 'u';
    int red, green, blue;
    red = ColorGetRed();
    green = ColorGetGreen();
    blue = ColorGetBlue();
    if (red > green && red > blue) {
        ret = 'r';
    }
    else if (green > red && green > blue) {
        ret = 'g';
    }
    else if (blue > green && blue > red) {
        ret = 'b';
    }

    return ret;
}