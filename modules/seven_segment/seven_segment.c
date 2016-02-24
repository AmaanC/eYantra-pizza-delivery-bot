#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>
#include "seven_segment.h"
#include "../lcd/lcd.h"

void SevenSegmentPinConfig() {
    // Port D upper nibble for CA connections
    // Port J for a,b,c,d, e,f,g,DEC as per manual
    // Both things above are in decreasing order
    // i.e.
    // Port J = Pin 7, 6, 5...0

    DDRD = DDRD | 0xF0; // set upper nibble of port D to output
    DDRJ = DDRJ | 0xFF; // set all bits to output
}

void SevenPortInit() {
    SevenSegmentPinConfig();
}

void SevenInitDevices() {
    cli(); //Clears the global interrupt
    SevenPortInit();  //Initializes all the ports
    sei();   // Enables the global interrupt
}

int SevenConvertToHex(int num) {
    int ret = 0x00;
    int values[10];
    values[0] = 0x03;
    values[1] = 0x9F;
    values[2] = 0x25;
    values[3] = 0x0D;
    values[4] = 0x99;
    values[5] = 0x49;
    values[6] = 0x41;
    values[7] = 0x1F;
    values[8] = 0x01;
    values[9] = 0x09;

    if (num >= 0 && num <= 9) {
        ret = values[num];
    }
    return ret;
}

void SevenDisplayNum(int num, int ca_num) {
    int digit;
    int select_value = 0x10 * (1 << (ca_num - 1));

    PORTD = PORTD & 0x0F; // Reset upper nibble to 0
    PORTD = PORTD | (unsigned char)select_value;

    digit = ((int)(num / pow(10, ca_num - 1))) % 10;
    PORTJ = SevenConvertToHex(digit);
}
