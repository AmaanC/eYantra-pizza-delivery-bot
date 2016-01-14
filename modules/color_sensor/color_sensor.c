#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "color_sensor.h"

volatile unsigned long int color_pulse = 0;
const int color_data_time = 100;


void color_pin_config() {
    DDRH = DDRH | 0xF0; // Set upper nibble of Port H as output
    DDRD = DDRD & 0xFE; // Set pin 0 of Port D as input (interrupt)
}

void color_interrupt_init() {
    // Enable INT0 (at D0)
    cli();
    EICRA = EICRA | 0x02; // INT0 is set to trigger with falling edge
    EIMSK = EIMSK | 0x01; // Enable Interrupt INT0 for color sensor
    sei();
}

ISR(INT0_vect) {
    color_pulse++;
}

int color_collect_pulses() {
    color_pulse = 0;
    _delay_ms(color_data_time);
    return color_pulse;
}

int color_get_red() {
    PORTH = PORTH & 0xBF; //set S2 low
    PORTH = PORTH & 0x7F; //set S3 low
    return color_collect_pulses();
}

int color_get_green() {
    PORTH = PORTH | 0x40; //set S2 High
    PORTH = PORTH | 0x80; //set S3 High
    return color_collect_pulses();
}

int color_get_blue() {
    PORTH = PORTH & 0xBF; //set S2 low
    PORTH = PORTH | 0x80; //set S3 High
    return color_collect_pulses();
}

void color_sensor_scaling() {
    PORTH = PORTH | 0x10; //set S0 high
    PORTH = PORTH | 0x20; //set S1 high
}

void color_init_devices() {
    color_pin_config();
    color_interrupt_init();
    color_sensor_scaling();
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
    red = color_get_red();
    green = color_get_green();
    blue = color_get_blue();
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
};