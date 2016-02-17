#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SharpSensor.h"
#include "../lcd/lcd.h"

int main() {
    int sharp;
    int value;
    int block_size = 0;

	LcdInitDevices();
	LcdSet4Bit();
	LcdInit();

    SharpInitDevices();
    while(1) {
        sharp = SharpAdcConversion(11);
        value = SharpGp2d12Estimation(sharp);
        block_size = SharpGetBlockSize(value, block_size); // Get block size from sharp sensor distance (in mm)
        LcdPrintf("Val: %d", value);
        _delay_ms(1000);
    }
}
