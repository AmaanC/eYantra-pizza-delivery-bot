#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "SharpSensor.h"
#include "../lcd/lcd.h"

int main() {
    char type_of_pizza;

	LcdInitDevices();
	LcdSet4Bit();
	LcdInit();

    SharpInitDevices();
    while(1) {
        type_of_pizza = SharpGetBlockType();
        
        LcdPrintf("pizza: %c", type_of_pizza);
        _delay_ms(1000);
    }
}
