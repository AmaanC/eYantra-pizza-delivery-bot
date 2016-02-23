#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "timer.h"
#include "../buzzer/buzzer.h"
#include "../seven_segment/seven_segment.h"

int time_count = 0;
void (*callback)();
int callback_delay = 0;
const int FREEZE_TIME = 5;
int frozen_time = -5;
char all_delivered = 0;

// TIMER4 initialize - prescale:1024
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1Hz
// actual value:  1.000Hz (0.0%)
void Timer4Init() {
    TCCR4B = 0x00; // stop
    TCNT4H = 0x1F; // Counter higher 8 bit value
    TCNT4L = 0x01; // Counter lower 8 bit value
    OCR4AH = 0x00; // Output Compair Register (OCR)- Not used
    OCR4AL = 0x00; // Output Compair Register (OCR)- Not used
    OCR4BH = 0x00; // Output Compair Register (OCR)- Not used
    OCR4BL = 0x00; // Output Compair Register (OCR)- Not used
    OCR4CH = 0x00; // Output Compair Register (OCR)- Not used
    OCR4CL = 0x00; // Output Compair Register (OCR)- Not used
    ICR4H  = 0x00; // Input Capture Register (ICR)- Not used
    ICR4L  = 0x00; // Input Capture Register (ICR)- Not used
    TCCR4A = 0x00; 
    TCCR4C = 0x00;
    TCCR4B = 0x04; // start Timer
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
ISR(TIMER4_OVF_vect) {
    // TIMER4 has overflowed
    TCNT4H = 0x1F; // reload counter high value
    TCNT4L = 0x01; // reload counter low value
    
    if (all_delivered == 0) {
        time_count++;
    }

    if (callback_delay > 0) {
        callback_delay--;
    }
    else if (callback != NULL) {
        callback();
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
    if (callback == NULL) {
        // Can't register callback
        return 0;
    }
    else {
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
    Timer4Init();
    TIMSK4 = 0x01; // timer4 overflow interrupt enable
    sei();   // Enables the global interrupts
}