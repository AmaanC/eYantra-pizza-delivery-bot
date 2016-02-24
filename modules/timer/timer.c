#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "timer.h"
#include "../buzzer/buzzer.h"
#include "../seven_segment/seven_segment.h"
#include "../lcd/lcd.h"

long int time_count = 0;

long int freq = 14745600;
int prescaler = 256;
int refresh_rate = 50;
int subtract_count;
int compa_val;
unsigned char compa_h;
unsigned char compa_l;
int compb_val;
unsigned char compb_h;
unsigned char compb_l;
int compc_val;
unsigned char compc_h;
unsigned char compc_l;


void (*callback)();
int callback_delay = 0;
const int FREEZE_TIME = 5;
int frozen_time = -5;
char all_delivered = 0;

void UpdateDisplay(int ca_num) {
    int actual_time = time_count / refresh_rate;
    if (actual_time - frozen_time < FREEZE_TIME) {
        SevenDisplayNum(frozen_time, ca_num);
    }
    else {
        SevenDisplayNum(actual_time, ca_num);
    }
}

// ((freq / 256) / 25).toString(16) = 0x0480
// 25Hz for COMPA
// COMPB and COMPC are called at intermediate values
void Timer3Init() {
    TCCR3B = 0x00; // stop
    TCNT3H = 0x00; // Counter higher 8 bit value
    TCNT3L = 0x00; // Counter lower 8 bit value
    OCR3AH = compa_h; // Output Compair Register (OCR)- Not used
    OCR3AL = compa_l; // Output Compair Register (OCR)- Not used
    OCR3BH = compb_h; // Output Compair Register (OCR)- Not used
    OCR3BL = compb_l; // Output Compair Register (OCR)- Not used
    OCR3CH = compc_h; // Output Compair Register (OCR)- Not used
    OCR3CL = compc_l; // Output Compair Register (OCR)- Not used
    ICR3H  = 0x00; // Input Capture Register (ICR)- Not used
    ICR3L  = 0x00; // Input Capture Register (ICR)- Not used
    TCCR3A = 0x00; 
    TCCR3C = 0x00;
    TCCR3B = 0x0C; // start Timer
}

ISR(TIMER3_COMPA_vect) {
    UpdateDisplay(1);

    if (all_delivered == 0) {
        time_count++;
    }
    if (callback_delay > 0) {
        callback_delay -= subtract_count;
    }
    else if (callback != NULL) {
        callback();
        callback = NULL;
    }
}

ISR(TIMER3_COMPB_vect) {
    UpdateDisplay(2);
}

ISR(TIMER3_COMPC_vect) {
    UpdateDisplay(3);
}

int GetCallbackTime() {
    return callback_delay;
}

void VictoryTime() {
    all_delivered = 1;
}

void FreezeDisplay() {
    frozen_time = time_count / refresh_rate;
}

// Call the registered function after a delay
// Delay needs to be in ms
// It works like this:
// callback_delay is set to (for example) 1000ms
// Our COMPA_Vect runs at 50Hz
// Therefore, 1000/50 is subtracted from callback_delay everytime
// our ISR is called
char RegisterCallback(void (*fn)(), short int delay) {
    if (callback != NULL) {
        // Can't register callback
        return 0;
    }
    else {
        // LcdPrintf("Registered");
        callback = fn;
        callback_delay = delay;
        return 1;
    }
}

short int GetCurrentTime() {
    return time_count / refresh_rate;
}

void ResetTime() {
    time_count = 0;
}

void InitTimer() {
    subtract_count = 1000 / refresh_rate;
    compa_val = (freq / prescaler) / refresh_rate;
    compa_h = compa_val >> 8;
    compa_l = compa_val & 0x00FF;
    compb_val = 2.0/3 * compa_val;
    compb_h = (compb_val) >> 8;
    compb_l = compb_val & 0x00FF;
    compc_val = 1.0/3 * compa_val;
    compc_h = (compc_val) >> 8;
    compc_l = compc_val & 0x00FF;
    SevenInitDevices();
    cli(); // Clears the global interrupts
    // Timer3Init();
    Timer3Init();
    // TIMSK3 = 0x01; // timer4 overflow interrupt enable
    TIMSK3 = 0x0E;
    sei();   // Enables the global interrupts
}