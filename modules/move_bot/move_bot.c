#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> 
#include "move_bot.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bl_sensor/bl_sensor.h"

unsigned char ADC_Value;
unsigned char Flag = 0;
unsigned char Left_Black_Line = 0;
unsigned char Center_Black_Line = 0;
unsigned char Right_Black_Line = 0;
unsigned char high_velocity = 255;
unsigned char low_velocity = 200;
volatile unsigned long int ShaftCounterLeft = 0; //to keep track of left position encoder 
volatile unsigned long int ShaftCounterRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

unsigned int temp = 0; //temporary variable to divide the degrees and distances so that the black like sensor 
unsigned int rem = 0;  //can check if it is at the correct path or not


void angle_rotate_left(unsigned int Degrees)
{
 float ReqdShaftCounter = 0;
 unsigned long int ReqdShaftCounterInt = 0;
 ReqdShaftCounter = (float) Degrees/ 4.090; 
 ReqdShaftCounterInt = (unsigned int) ReqdShaftCounter;
 ShaftCounterRight = 0; 
 ShaftCounterLeft = 0; 
 while (1)
 {
 	Center_Black_Line = bl_sensor_ADC_Conversion(2);
 	Right_Black_Line = bl_sensor_ADC_Conversion(1);
  	if(((ShaftCounterRight >= ReqdShaftCounterInt) | (ShaftCounterLeft >= ReqdShaftCounterInt)) | ( Center_Black_Line > 0x28))
  		break;
 }
  pos_encoder_stop(); 
}

void angle_rotate_right(unsigned int Degrees)
{
 float ReqdShaftCounter = 0;
 unsigned long int ReqdShaftCounterInt = 0;
 ReqdShaftCounter = (float) Degrees/ 4.090; 
 ReqdShaftCounterInt = (unsigned int) ReqdShaftCounter;
 ShaftCounterRight = 0; 
 ShaftCounterLeft = 0; 
 while (1)
 {
 	Left_Black_Line = bl_sensor_ADC_Conversion(3);
	Center_Black_Line = bl_sensor_ADC_Conversion(2);
  	if(((ShaftCounterRight >= ReqdShaftCounterInt) | (ShaftCounterLeft >= ReqdShaftCounterInt)) | ( Center_Black_Line > 0x28))
  		break;
 }
  pos_encoder_stop(); 
}

void move_bot_init_devices(void){
	bl_sensor_init_devices();
	pos_encoder_init_devices();
	pos_encoder_timer5_init();
}

void rotate_bot(int Degrees){
    if(Degrees > 0){
    	pos_encoder_left();
    	angle_rotate_left(Degrees);
    }
    else if (Degrees < 0 ){
    	pos_encoder_right();
    	angle_rotate_right(Degrees);
    }
}

void move_bot_forward_mm(unsigned int DistanceInMM)
{
 float ReqdShaftCounter = 0;
 unsigned long int ReqdShaftCounterInt = 0;

 ReqdShaftCounter = DistanceInMM / 5.338; 
 ReqdShaftCounterInt = (unsigned long int) ReqdShaftCounter;
  
 ShaftCounterRight = 0;
 ShaftCounterLeft = 0;
 while(1)
 {
  if((ShaftCounterRight > ReqdShaftCounterInt) | (ShaftCounterLeft > ReqdShaftCounterInt))
  {
  	break;
  }
  else {
  		Flag = 0;
		Left_Black_Line = bl_sensor_ADC_Conversion(3);
		Center_Black_Line = bl_sensor_ADC_Conversion(2);	
		Right_Black_Line = bl_sensor_ADC_Conversion(1);	

		if(Center_Black_Line > 0x28){
			Flag = 1;
			pos_encoder_velocity(high_velocity, high_velocity);
		}

		if((Left_Black_Line < 0x28) && (Flag==0)){
			Flag=1;
			pos_encoder_velocity(high_velocity, low_velocity);
		}

		if((Right_Black_Line < 0x28) && (Flag==0)){
			Flag=1;
			pos_encoder_velocity(low_velocity, high_velocity);
		}

		if(Center_Black_Line < 0x28 && Left_Black_Line < 0x28 && Right_Black_Line < 0x28){
			rotate_bot(360);
		}
    }
 } 
  pos_encoder_stop(); 
}

void move_bot_forward(int Distance){
	pos_encoder_forward();
	move_bot_forward_mm(Distance);
}
