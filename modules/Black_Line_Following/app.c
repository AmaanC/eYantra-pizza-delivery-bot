#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <math.h> //included to support power function
#include "lcd.c"
#include "Black_Line_Following.h"

//Main Function
int main()
{
	black_line_following_init_devices();
	lcd_set_4bit();
	lcd_init();
	
	while(1)
	{

		Left_black_line = black_line_following_ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_black_line = black_line_following_ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_black_line = black_line_following_ADC_Conversion(1);	//Getting data of Right WL Sensor

		flag=0;

		black_line_following_print_sensor(1,1,3);	//Prints value of black Line Sensor1
		black_line_following_print_sensor(1,5,2);	//Prints Value of black Line Sensor2
		black_line_following_print_sensor(1,9,1);	//Prints Value of black Line Sensor3
		
		

		if(Center_black_line>0x28)
		{
			flag=1;
			black_line_following_forward();
			black_line_following_velocity(150,150);
		}

		if((Left_black_line<0x28) && (flag==0))
		{
			flag=1;
			black_line_following_forward();
			black_line_following_velocity(130,50);
		}

		if((Right_black_line<0x28) && (flag==0))
		{
			flag=1;
			black_line_following_forward();
			black_line_following_velocity(50,130);
		}

		if(Center_black_line<0x28 && Left_black_line<0x28 && Right_black_line<0x28)
		{
			black_line_following_forward();
			black_line_following_velocity(0,0);
		}

	}
}
