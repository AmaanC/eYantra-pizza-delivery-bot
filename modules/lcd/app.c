
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

//Main Function
int main(void) {
	LcdInitDevices();
	LcdSet4Bit();
	LcdInit();
	_delay_ms(1000);
	while(1)
	{
		LcdPrintf("DANGER ZONE! %d", 42);
		// Lcdwr_char('a');
		// Lcdwr_char('b');
		_delay_ms(1000);
		LcdPrintf("%s", "HAL 9000");
		_delay_ms(1000);
		// Lcdwr_command(0x01); //Clear the LCD
	}
}
