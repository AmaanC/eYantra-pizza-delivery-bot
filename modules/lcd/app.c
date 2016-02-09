
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
	while(1)
	{
		lcd_cursor(1,3);
		lcd_string("FIRE BIRD 5");
		lcd_cursor(2,1);
		lcd_string("NEX ROBOTICS IND");
	}
}
