#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <math.h> //included to support power function
#include "lcd.c"
#include "White_Line_Following.h"

//Main Function
int main()
{
	white_line_following_init_devices();
	lcd_set_4bit();
	lcd_init();
	
	while(1)
	{

		Left_white_line = white_line_following_ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_white_line = white_line_following_ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_white_line = white_line_following_ADC_Conversion(1);	//Getting data of Right WL Sensor

		flag=0;

		white_line_following_print_sensor(1,1,3);	//Prints value of White Line Sensor1
		white_line_following_print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		white_line_following_print_sensor(1,9,1);	//Prints Value of White Line Sensor3
		
		

		if(Center_white_line>0x28)
		{
			flag=1;
			white_line_following_forward();
			white_line_following_velocity(150,150);
		}

		if((Left_white_line<0x28) && (flag==0))
		{
			flag=1;
			white_line_following_forward();
			white_line_following_velocity(130,50);
		}

		if((Right_white_line<0x28) && (flag==0))
		{
			flag=1;
			white_line_following_forward();
			white_line_following_velocity(50,130);
		}

		if(Center_white_line<0x28 && Left_white_line<0x28 && Right_white_line<0x28)
		{
			white_line_following_forward();
			white_line_following_velocity(0,0);
		}

	}
}
