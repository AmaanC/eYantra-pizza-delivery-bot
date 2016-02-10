#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "motor.h"
#include "../buzzer/buzzer.h"
#include "../pos_encoder/pos_encoder.h"

//Main Function
int main()
{
    init_devices();
    // 210, 255 for curve
    velocity (255, 255);
    // Try different valuse between 0 to 255
    while(1)
    {
    
        pos_encoder_forward_mm(100); //both wheels forward
        pos_encoder_stop();
        _delay_ms(500);

        buzzer_beep(100);
        stop();                     
        _delay_ms(5000);
    }
}