#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../buzzer/buzzer.h"

int time_count = 0;

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

// This ISR can be used to schedule events like refreshing ADC data, LCD data
ISR(TIMER4_OVF_vect) {
    // TIMER4 has overflowed
    TCNT4H = 0x1F; // reload counter high value
    TCNT4L = 0x01; // reload counter low value
    time_count++;
}

int GetCurrentTime() {
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