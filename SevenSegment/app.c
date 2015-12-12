#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SevenSegment.h"

//Main
int main(void)
{
    init_devices();
    while(1)
    {
       display_num(12);
       _delay_ms(1);
   }
}
