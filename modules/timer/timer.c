#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "timer.h"
#include "../buzzer/buzzer.h"
#include "../seven_segment/seven_segment.h"
#include "../lcd/lcd.h"

int time_count = 0;
void (*callback)();
int callback_delay = 0;
const int FREEZE_TIME = 5;
int frozen_time = -5;
char all_delivered = 0;

// TIMER3 initialize - prescale:256
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1Hz
// actual value:  1.000Hz (0.0%)
void Timer3Init() {
    TCCR3B = 0x00; // stop
    TCNT3H = 0x1F; // Counter higher 8 bit value
    TCNT3L = 0x01; // Counter lower 8 bit value
    OCR3AH = 0x00; // Output Compair Register (OCR)- Not used
    OCR3AL = 0x00; // Output Compair Register (OCR)- Not used
    OCR3BH = 0x00; // Output Compair Register (OCR)- Not used
    OCR3BL = 0x00; // Output Compair Register (OCR)- Not used
    OCR3CH = 0x00; // Output Compair Register (OCR)- Not used
    OCR3CL = 0x00; // Output Compair Register (OCR)- Not used
    ICR3H  = 0x00; // Input Capture Register (ICR)- Not used
    ICR3L  = 0x00; // Input Capture Register (ICR)- Not used
    TCCR3A = 0x00; 
    TCCR3C = 0x00;
    TCCR3B = 0x04; // start Timer
}

void UpdateDisplay() {
    if (time_count - frozen_time < FREEZE_TIME) {
        SevenDisplayNum(frozen_time);
    }
    else {
        SevenDisplayNum(time_count);
    }
}

// This ISR can be used to schedule events like refreshing ADC data, LCD data
ISR(TIMER3_OVF_vect) {
    // TIMER3 has overflowed
    TCNT3H = 0x1F; // reload counter high value
    TCNT3L = 0x01; // reload counter low value
    
    if (all_delivered == 0) {
        time_count++;
    }
}

// (0xFFFF - (one_sec / 10)).toString(16)
void Timer5Init() {
    TCCR5B = 0x00; // stop
    TCNT5H = 0x9A; // Counter higher 8 bit value
    TCNT5L = 0xFB; // Counter lower 8 bit value
    OCR5AH = 0x00; // Output Compair Register (OCR)- Not used
    OCR5AL = 0x00; // Output Compair Register (OCR)- Not used
    OCR5BH = 0x00; // Output Compair Register (OCR)- Not used
    OCR5BL = 0x00; // Output Compair Register (OCR)- Not used
    OCR5CH = 0x00; // Output Compair Register (OCR)- Not used
    OCR5CL = 0x00; // Output Compair Register (OCR)- Not used
    ICR5H  = 0x00; // Input Capture Register (ICR)- Not used
    ICR5L  = 0x00; // Input Capture Register (ICR)- Not used
    TCCR5A = 0x00; 
    TCCR5C = 0x00;
    TCCR5B = 0x04; // start Timer
}

ISR(TIMER5_OVF_vect) {
    TCNT5H = 0x9A;
    TCNT5L = 0xFB;

    if (callback_delay > 0) {
        callback_delay--;
    }
    else if (callback != NULL) {
        callback();
        callback = NULL;
    }
    UpdateDisplay();
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
    return time_count;
}

void ResetTime() {
    time_count = 0;
}

void InitTimer() {
    cli(); // Clears the global interrupts
    Timer3Init();
    Timer5Init();
    TIMSK3 = 0x01; // timer4 overflow interrupt enable
    TIMSK5 = 0x01;
    sei();   // Enables the global interrupts
}