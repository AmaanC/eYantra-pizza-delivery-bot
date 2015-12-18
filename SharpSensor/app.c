#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SharpSensor.h"

int main()
{
    int sharp;
    int value;
    int block_size = 0;
    sharp_init_devices();
    while(1) {
        sharp = sharp_ADC_Conversion(11);
        value = sharp_GP2D12_estimation(sharp);
        block_size = sharp_get_block_size(value, block_size); // Get block size from sharp sensor distance (in mm)
    }
}
