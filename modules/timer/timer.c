#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "timer.h"
#include "../buzzer/buzzer.h"
#include "../seven_segment/seven_segment.h"
#include "../lcd/lcd.h"

long int time_count = 0;
int refresh_rate = 50;
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

// ((freq / 256) / 50).toString(16) = 0x0480
// 50Hz for COMPA
// COMPB and COMPC are called at intermediate values
void Timer3Init() {
    TCCR3B = 0x00; // stop
    TCNT3H = 0x00; // Counter higher 8 bit value
    TCNT3L = 0x00; // Counter lower 8 bit value
    OCR3AH = 0x04; // Output Compair Register (OCR)- Not used
    OCR3AL = 0x80; // Output Compair Register (OCR)- Not used
    OCR3BH = 0x03; // Output Compair Register (OCR)- Not used
    OCR3BL = 0x00; // Output Compair Register (OCR)- Not used
    OCR3CH = 0x01; // Output Compair Register (OCR)- Not used
    OCR3CL = 0x80; // Output Compair Register (OCR)- Not used
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
    frozen_time = time_count;
}

// Call the registered function after a delay
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
    SevenInitDevices();
    cli(); // Clears the global interrupts
    // Timer3Init();
    Timer3Init();
    // TIMSK3 = 0x01; // timer4 overflow interrupt enable
    TIMSK3 = 0x0E;
    sei();   // Enables the global interrupts
}