#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> 
#include "move_bot.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bl_sensor/bl_sensor.h"
#include "../lcd/lcd.h"

#define LEFT 3
#define CENTER 2
#define RIGHT 1

unsigned char ADC_value;
unsigned char left_black_line = 0;
unsigned char center_black_line = 0;
unsigned char right_black_line = 0;
unsigned char high_velocity = 100;
unsigned char low_velocity = 70;
unsigned int degrees; //to accept angle in degrees for turning

const int rotation_threshold = 5; // in degrees

float p_gain = 200;
float i_gain = 0.2;
float d_gain = 120;

int32_t integral_error = 0;
int32_t prev_error = 0;

float control;
float sensor;
float prev_sensor;

void MoveBotInitDevices() {
    BlSensorInitDevices();
    PosEncoderInitDevices();
    PosEncoderTimer5Init();
}

float PidControl(float curve_value, float required_value) {
    float pid;
    float error;    

    error = required_value - curve_value;
    pid = (p_gain * error)  + (i_gain * integral_error) + (d_gain * (error - prev_error)); 

    integral_error += error;               // integral is simply a summation over time
    prev_error = error;                    // save previous for derivative  

    return pid;
}

int IsBlack(unsigned char sensor_vlaue) {
    if (sensor_vlaue > 0x28) 
        return 1;
    else 
        return 0;
 }

float ReadSensors() {
   unsigned char right, center, left;
   unsigned char sensor1, sensor2, sensor3;

   float avg_sensor = 0.0;

   right = BlSensorAdcConversion(RIGHT);
   if(IsBlack(right)) {
        sensor3 = 1; // Right black line sensor
   }
   else {
        sensor3 = 0;
   }

   center = BlSensorAdcConversion(CENTER);
   if(IsBlack(center)) {
        sensor2 = 1; // Center black line sensor
   }
   else {
        sensor2 = 0;
   }

   left=BlSensorAdcConversion(LEFT);
   if(IsBlack(left)) {
        sensor1 = 1; // Left black line sensor
   }
   else {
        sensor1 = 0;
   }

   if(sensor1==0 && sensor2==0 && sensor3==0) {
        return 0xFF;
   }

   // Calculate weighted mean
   avg_sensor = (float) sensor1*1 + sensor2*2 + sensor3*3 ;
   avg_sensor = (float) avg_sensor / (sensor1 + sensor2 + sensor3 );
   return avg_sensor;
}

int AngleRotate(unsigned int degrees) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    float min_shaft_count = 0;
    unsigned long int min_shaft_count_int = 0;
    reqd_shaft_counter = (float) (degrees) / 4.090;
    reqd_shaft_counter_int = (unsigned int) reqd_shaft_counter;
  
    min_shaft_count = (float) (degrees - rotation_threshold) / 4.090;
    min_shaft_count_int = (unsigned int) min_shaft_count;
    ResetLeftShaft(); 
    ResetRightShaft(); 
    while (1) {
        center_black_line = BlSensorAdcConversion(2);
        // We stop in the following conditions:
        // - If the pos encoder says we've reached
        // - If the bl sensor says we've reached
        // If we rotate too far, and still don't see a black line, let's rotate back
        if(
            ((GetShaftCountRight() >= reqd_shaft_counter_int) | (GetShaftCountLeft() >= reqd_shaft_counter_int)) ||
            (IsBlack(center_black_line) && (GetShaftCountRight() >= min_shaft_count_int || GetShaftCountLeft() >= min_shaft_count_int))
        ) {
            break;
        }
    }
    // If we see a black line, then this is the correct place to have stopped
    // Else, we've probably gone too far, and should return back to exactly the degrees that were passed in
    if (IsBlack(center_black_line)) {
        return 0;
    }
    else {
        return 1;
    }
    PosEncoderStop(); 
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
    PosEncoderStop();
}

void MoveBot(int distance_in_mm) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    reqd_shaft_counter = distance_in_mm / 5.338; 
    reqd_shaft_counter_int = (unsigned long int) reqd_shaft_counter;
    ResetRightShaft();
    ResetLeftShaft();

    while(1)
    {
        if((GetShaftCountRight() > reqd_shaft_counter_int) | (GetShaftCountLeft() > reqd_shaft_counter_int)) {
            break;
        }

        else {
            //Take current sensor reading
            //return value is between 0 to 5
            //When the line is towards right of center then value tends to 5
            //When the line is towards left of center then value tends to 1
            //When line is in the exact center the the value is 3
            sensor = ReadSensors();
    
            //If line is not found beneath any sensor, use last sensor value. 
            if(sensor == 0xFF)
            {
               sensor = prev_sensor;
            }    
    
            //PID Algorithm generates a control variable from the current value
            //and the required value. Since the aim is to keep the line always
            //beneath the center sensor so the required value is 2 (second parameter)
            //The first argument is the current sensor reading.
            //The more the difference between the two greater is the control variable.
            //This control variable is used to produce turning in the robot.
            //When current value is close to required value is close to 0.
    
            control = PidControl(sensor,2.0);    
            //Limit the control
            if(control > 510)
                control = 510;
            if(control < -510)
                control = -510;    
            if(control < 0.0)//the left sensor sees the line so we must turn right
            {
               if(control > 255)
                  PosEncoderVelocity(255, control-255);
               else
                  PosEncoderVelocity(255, 255-control);    
            }
            if(control >= 0.0)//the right sensor sees the line so we must turn left
            {
               if(control < -255)
                  PosEncoderVelocity(-(control + 255), 255);
               else
                  PosEncoderVelocity(255 + control, 255);    
            }    
            //Delay     
            _delay_ms(10);    
            prev_sensor = sensor;
        }
    }
    PosEncoderStop();
}

void MoveBotForward(int distance) {
    PosEncoderForward();
    MoveBot(distance);
}

void MoveBotReverse(int distance) {
    PosEncoderBack();
    MoveBot(distance);
}