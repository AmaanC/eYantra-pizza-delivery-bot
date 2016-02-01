#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "motor.h"

//Main Function
int main()
{
    init_devices();
    velocity (126.25905331, 153.89310331);
    // Try different valuse between 0 to 255
    while(1)
    {
    
        forward(); //both wheels forward
        _delay_ms(30000);

        stop();                     
        _delay_ms(1000);
    }
}