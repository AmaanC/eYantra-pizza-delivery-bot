#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include "color_sensor.h"

char get_color() {
    // It will return the following characters:
    // u by default (if something went wrong)
    // r for red
    // g for green
    // b for blue
    
    char ret = 'u';
};