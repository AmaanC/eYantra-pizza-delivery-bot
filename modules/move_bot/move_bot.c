#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> 
#include "move_bot.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bl_sensor/bl_sensor.h"

unsigned char ADC_value;
unsigned char Flag = 0;
unsigned char left_black_line = 0;
unsigned char center_black_line = 0;
unsigned char right_black_line = 0;
unsigned char high_velocity = 255;
unsigned char low_velocity = 200;
volatile unsigned long int ShaftCounterLeft = 0; //to keep track of left position encoder 
volatile unsigned long int ShaftCounterRight = 0; //to keep track of right position encoder
unsigned int degrees; //to accept angle in degrees for turning

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
int AngleRotate(unsigned int degrees) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    float min_shaft_count = 0;
    unsigned long int min_shaft_count_int = 0;
    reqd_shaft_counter = (float) (degrees + rotation_threshold) / 4.090;
    reqd_shaft_counter_int = (unsigned int) reqd_shaft_counter;
  
    min_shaft_count = (float) (degrees - rotation_threshold) / 4.090;
    min_shaft_count_int = (unsigned int) min_shaft_count;
  
    ShaftCounterRight = 0; 
    ShaftCounterLeft = 0; 
    while (1) {
 	      center_black_line = BlSensorAdcConversion(2);
        // We stop in the following conditions:
        // - If the pos encoder says we've reached
        // - If the bl sensor says we've reached
        // If we rotate too far, and still don't see a black line, let's rotate back
         if(
            ((ShaftCounterRight >= reqd_shaft_counter_int) | (ShaftCounterLeft >= reqd_shaft_counter_int)) ||
            (center_black_line > 0x28 && (ShaftCounterRight >= min_shaft_count_int || ShaftCounterLeft >= min_shaft_count_int))
        ) {
         	    break;
        }
 }
    // If we see a black line, then this is the correct place to have stopped
    // Else, we've probably gone too far, and should return back to exactly the degrees that were passed in
    if (center_black_line > 0x28) {
        return 0;
    }
    else {
        return 1;
    }
    PosEncoderStop(); 
}

void GetSensorsStatus() {
    left_black_line = BlSensorAdcConversion(3);
    center_black_line = BlSensorAdcConversion(2);    
    right_black_line = BlSensorAdcConversion(1);

}

void MoveBotInitDevices(void) {
	BlSensorInitDevices();
	PosEncoderInitDevices();
	PosEncoderTimer5Init();
}

void RotateBot(int degrees) {
    int too_far = 0;
    if(degrees > 0){
    	PosEncoderLeft();
    	too_far = AngleRotate(degrees);
        if (too_far == 1) {
            PosEncoderRight();
            PosEncoderAngleRotate(rotation_threshold);
        }
    }
    else if (degrees < 0 ){
    	PosEncoderRight();
        too_far = AngleRotate(degrees);
        if (too_far == 1) {
            PosEncoderLeft();
            PosEncoderAngleRotate(rotation_threshold);
        }
    }
}

static int right_turn = 0;
static int left_turn = 0;


void MoveBotForwardMm(unsigned int distance_in_mm) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
   
    reqd_shaft_counter = distance_in_mm / 5.338; 
    reqd_shaft_counter_int = (unsigned long int) reqd_shaft_counter;
     
    right_turn = 0;
    left_turn = 0;
    ShaftCounterRight = 0;
    ShaftCounterLeft = 0;
    while(1)
    {
     if((ShaftCounterRight > reqd_shaft_counter_int) | (ShaftCounterLeft > reqd_shaft_counter_int)) {
     	   break;
     }
     else {
        GetSensorsStatus();
     		 Flag = 0;	

	   if(center_black_line > 0x28) {
	   		 Flag = 1;
	   		 PosEncoderVelocity(high_velocity, high_velocity);
	   	}
   
	   if((left_black_line < 0x28) && (Flag==0)) {
	   		 Flag=1;
	   		 PosEncoderVelocity(high_velocity, low_velocity);
             right_turn++;
             if(right_turn == 2) {
                PosEncoderVelocity(low_velocity, high_velocity);
             }
             else if(right_turn == 3) {
                PosEncoderVelocity(high_velocity, high_velocity);
             }
	   	}
   
	   if((right_black_line < 0x28) && (Flag==0)) {
	   		 Flag=1;
	   		 PosEncoderVelocity(low_velocity, high_velocity);
             left_turn++;
             if(left_turn == 2) {
                PosEncoderVelocity(high_velocity, low_velocity);
             }
             else if(left_turn == 3) {
                PosEncoderVelocity(high_velocity, high_velocity);
             }
	    }
   
	   if(center_black_line < 0x28 && left_black_line < 0x28 && right_black_line < 0x28){
	   		RotateBot(360);
	   	}
    }
  } 
     PosEncoderStop(); 
}

void MoveBotForward(int distance) {
    PosEncoderForward();
	  MoveBotForwardMm(distance);
}
