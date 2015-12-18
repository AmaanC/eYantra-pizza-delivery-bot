//#define __OPTIMIZE__ -O0
#define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> //included to support power function

#include "../SharpSensor/SharpSensor.h"
#include "../SevenSegment/SevenSegment.h"
// These are the header files. They're linked together using the Makefile

int main()
{
    int sharp;
    int value;
    int block_size = 0;
    sharp_init_devices();
    seven_init_devices();
    while(1) {
        sharp = sharp_ADC_Conversion(11);
        value = sharp_GP2D12_estimation(sharp);
        block_size = sharp_get_block_size(value, block_size); // Get block size from sharp sensor distance (in mm)

        seven_display_num(block_size);
        _delay_us(500);
    }
}
