#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> //included to support power function
#include "Bl_sensor.h"

//Main Function
int main()
{
	bl_sensor_init_devices();
	unsigned char Left_black_line = 0;
	unsigned char Center_black_line = 0;
	unsigned char Right_black_line = 0;
	int flag;
	
	while(1)
	{

		Left_black_line = bl_sensor_ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_black_line = bl_sensor_ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_black_line = bl_sensor_ADC_Conversion(1);	//Getting data of Right WL Sensor

		flag=0;		

		if(Center_black_line>0x28)
		{
			flag=1;
		}

		if((Left_black_line<0x28) && (flag==0))
		{
			flag=1;
		}

		if((Right_black_line<0x28) && (flag==0))
		{
			flag=1;
		}

		if(Center_black_line<0x28 && Left_black_line<0x28 && Right_black_line<0x28)
		{
		}

	}
}
