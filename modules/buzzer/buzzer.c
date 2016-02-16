// #define __OPTIMIZE__ -O0
// #define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "buzzer.h"
#include "../custom_delay/custom_delay.h"

//Function to initialize Buzzer 
void BuzzerPinConfig() {
    DDRC = DDRC | 0x08;		//Setting PORTC 3 as output
    PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

void BuzzerPortInit() {
    BuzzerPinConfig();
}

void BuzzerOn() {
    unsigned char port_restore = 0;
    port_restore = PINC;
    port_restore = port_restore | 0x08;
    PORTC = port_restore;
}

void BuzzerOff() {
    unsigned char port_restore = 0;
    port_restore = PINC;
    port_restore = port_restore & 0xF7;
    PORTC = port_restore;
}

void BuzzerBeep(int ms) {
    BuzzerOn();
    CustomDelay(ms);
    BuzzerOff();
    CustomDelay(ms);
}

void BuzzerInitDevices() {
    cli(); //Clears the global interrupts
    BuzzerPortInit();
    sei(); //Enables the global interrupts
}