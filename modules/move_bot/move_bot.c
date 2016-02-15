#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> 
#include "move_bot.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bl_sensor/bl_sensor.h"

unsigned char ADC_Value;
unsigned char flag = 0;
unsigned char Left_black_line = 0;
unsigned char Center_black_line = 0;
unsigned char Right_black_line = 0;
unsigned char max_velocity = 255;

volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

void move_bot_init_devices(){
	bl_sensor_init_devices();
	pos_encoder_init_devices();
	pos_encoder_timer5_init();
}

void rotate_bot(int Degrees){
	Left_black_line = black_line_following_ADC_Conversion(3);
	Center_black_line = black_line_following_ADC_Conversion(2);
	Right_black_line = black_line_following_ADC_Conversion(1);	

    if(Degrees > 0 && Right_black_line > 0x28)
    {
      pos_encoder_left_degrees(abs(Degrees));
      pos_encoder_stop();     
      // _delay_ms(500); 
    }
    else if (Degrees < 0 && Left_black_line > 0x28){
      pos_encoder_right_degrees(abs(Degrees));
      pos_encoder_stop();     
      // _delay_ms(500); 
    }

}

void move_bot_forward(unsigned int distance){
	Left_black_line = black_line_following_ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_black_line = black_line_following_ADC_Conversion(2);	//Getting data of Center WL Sensor
	Right_black_line = black_line_following_ADC_Conversion(1);	//Getting data of Right WL Sensor

	if(Center_black_line>0x28)
		{
			flag=1;
			pos_encoder_velocity(max_velocity, max_velocity);
			pos_encoder_forward_mm(distance);
			_delay_ms(500);
		}
}

void move_bot_back(unsigned int distance){
	Left_black_line = black_line_following_ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_black_line = black_line_following_ADC_Conversion(2);	//Getting data of Center WL Sensor
	Right_black_line = black_line_following_ADC_Conversion(1);	//Getting data of Right WL Sensor

	if(Center_black_line>0x28)
		{
			flag=1;
			pos_encoder_velocity(max_velocity, max_velocity);
			pos_encoder_back_mm(distance);
			_delay_ms(500);
		}
}