
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

//Main Function
int main(void)
{
	lcd_init_devices();
	lcd_set_4bit();
	lcd_init();
	_delay_ms(1000);
	while(1)
	{
		lcd_cursor(0,0);
		lcd_string("DANGER ZONE!");
		// lcd_wr_char('a');
		// lcd_wr_char('b');
		_delay_ms(1000);
		lcd_wr_command(0x01); //Clear the LCD
	}
}
