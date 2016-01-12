#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "timer.h"
#include "../SevenSegment/SevenSegment.h"

void timer_1s_magic() {

}

//Main Function
int main(void)
{
    timer_init_devices();
    while(1);
}
