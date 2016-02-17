#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "motor.h"
#include "../buzzer/buzzer.h"
#include "../pos_encoder/pos_encoder.h"

//Main Function
int main() {
    InitDevices();
    // 210, 255 for curve
    Velocity (255, 255);
    // Try different valuse between 0 to 255
    while(1)
    {
    
        PosEncoderForwardMm(100); //both wheels forward
        PosEncoderStop();
        _delay_ms(500);

        BuzzerBeep(100);
        Stop();                     
        _delay_ms(5000);
    }
}