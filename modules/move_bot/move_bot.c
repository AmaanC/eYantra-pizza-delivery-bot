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

const int rotation_threshold = 5; // in degrees

// This function will stop the bot in a range of +- rotation_threshold
// For eg:
// If you pass 90 degrees, but the line is at 87 degrees, it'll stop there
// If the line is at 92 degrees, it'll stop there
// If you're turning on white entirely, it'll turn 95 degrees, and then turn back 5 degrees
// because it didn't find a black line at all
// To indicate that it's turned too far, we return 1
// Else we return 0
int angle_rotate(unsigned int Degrees)
{
 float ReqdShaftCounter = 0;
 unsigned long int ReqdShaftCounterInt = 0;
 float MinShaftCount = 0;
 unsigned long int MinShaftCountInt = 0;
 ReqdShaftCounter = (float) (Degrees + rotation_threshold) / 4.090;
 ReqdShaftCounterInt = (unsigned int) ReqdShaftCounter;

 MinShaftCount = (float) (Degrees - rotation_threshold) / 4.090;
 MinShaftCountInt = (unsigned int) MinShaftCount;

 ShaftCounterRight = 0; 
 ShaftCounterLeft = 0; 
 while (1)
 {
 	Center_Black_Line = bl_sensor_ADC_Conversion(2);
    // We stop in the following conditions:
    // - If the pos encoder says we've reached
    // - If the bl sensor says we've reached
    // If we rotate too far, and still don't see a black line, let's rotate back
  	if(
        ((ShaftCounterRight >= ReqdShaftCounterInt) | (ShaftCounterLeft >= ReqdShaftCounterInt)) ||
        (Center_Black_Line > 0x28 && (ShaftCounterRight >= MinShaftCountInt || ShaftCounterLeft >= MinShaftCountInt))
    ) {
  		break;
    }
 }
 // If we see a black line, then this is the correct place to have stopped
 // Else, we've probably gone too far, and should return back to exactly the degrees that were passed in
 if (Center_Black_Line > 0x28) {
    return 0;
 }
 else {
    return 1;
 }
  pos_encoder_stop(); 
}

void move_bot_init_devices(void){
	bl_sensor_init_devices();
	pos_encoder_init_devices();
	pos_encoder_timer5_init();
}

void rotate_bot(int Degrees){
    int too_far = 0;
    if(Degrees > 0){
    	pos_encoder_left();
    	too_far = angle_rotate(Degrees);
        if (too_far == 1) {
            pos_encoder_right();
            pos_encoder_angle_rotate(rotation_threshold);
        }
    }
    else if (Degrees < 0 ){
    	pos_encoder_right();
        too_far = angle_rotate(Degrees);
        if (too_far == 1) {
            pos_encoder_left();
            pos_encoder_angle_rotate(rotation_threshold);
        }
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
