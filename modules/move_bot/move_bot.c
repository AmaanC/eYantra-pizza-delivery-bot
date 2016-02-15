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
unsigned char max_velocity = 255;

volatile unsigned long int ShaftCounterLeft = 0; //to keep track of left position encoder 
volatile unsigned long int ShaftCounterRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

unsigned int temp = 0; //temporary variable to divide the degrees and distances so that the black like sensor 
unsigned int rem = 0;  //can check if it is at the correct path or not

void move_bot_init_devices(void){
	bl_sensor_init_devices();
	pos_encoder_init_devices();
	pos_encoder_timer5_init();
}

void rotate_bot(int Degrees){
	Left_Black_Line = bl_sensor_ADC_Conversion(3);
	Center_Black_Line = bl_sensor_ADC_Conversion(2);
	Right_Black_Line = bl_sensor_ADC_Conversion(1);	

    if(Degrees > 0){
    	temp = Degrees;
    	for(rem = 10; rem <= temp; rem += 10){
    		Center_Black_Line = bl_sensor_ADC_Conversion(2);
			Right_Black_Line = bl_sensor_ADC_Conversion(1);	
    		pos_encoder_left_degrees(abs(rem)); 

    		if(Right_Black_Line < 0x28 && Center_Black_Line < 0x28)
    			continue;
    		else
    			break;
    	}
    	pos_encoder_stop();
    }
    else if (Degrees < 0 ){
    	temp = Degrees;
    	for(rem = 10; rem <= temp; rem += 10){
    		Left_Black_Line = bl_sensor_ADC_Conversion(3);
			Center_Black_Line = bl_sensor_ADC_Conversion(2);
    		pos_encoder_right_degrees(abs(rem)); 	

    		if(Left_Black_Line < 0x28 && Center_Black_Line < 0x28)
    			continue;
    		else
    			break;
    	}
    	pos_encoder_stop();
    }
}

void move_bot_forward(unsigned int distance){
	Left_Black_Line = bl_sensor_ADC_Conversion(3);	//Getting data of Left WL Sensor
	Center_Black_Line = bl_sensor_ADC_Conversion(2);	//Getting data of Center WL Sensor
	Right_Black_Line = bl_sensor_ADC_Conversion(1);	//Getting data of Right WL Sensor

	for(rem = 10; rem <= distance; rem += 10){

		Flag = 0;
		Left_Black_Line = bl_sensor_ADC_Conversion(3);	//Getting data of Left WL Sensor
		Center_Black_Line = bl_sensor_ADC_Conversion(2);	//Getting data of Center WL Sensor
		Right_Black_Line = bl_sensor_ADC_Conversion(1);	//Getting data of Right WL Sensor

		if(Center_Black_Line > 0x28){
			Flag = 1;
			pos_encoder_velocity(max_velocity, max_velocity);
			pos_encoder_forward_mm(rem);
		}

		if((Left_Black_Line < 0x28) && (Flag==0)){
			Flag=1;
			pos_encoder_velocity(70,50);
			pos_encoder_forward_mm(rem);
		}

		if((Right_Black_Line < 0x28) && (Flag==0)){
			Flag=1;
			pos_encoder_velocity(50,70);
			pos_encoder_forward_mm(rem);
		}

		if(Center_Black_Line < 0x28 && Left_Black_Line < 0x28 && Right_Black_Line < 0x28){
			rotate_bot(360);
		}
	}
}