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

#define TRUE 1
#define FALSE 0

unsigned char ADC_value;
unsigned char left_black_line = 0;
unsigned char center_black_line = 0;
unsigned char right_black_line = 0;
unsigned char high_velocity = 255;
unsigned char low_velocity = 230;
unsigned int degrees; //to accept angle in degrees for turning

const int rotation_threshold = 20; // in degrees

void MoveBotInitDevices() {
    BlSensorInitDevices();
    PosEncoderInitDevices();
    PosEncoderTimer5Init();
}

int IsBlack(int sensor_num) {
    if (BlSensorAdcConversion(sensor_num) > 0x28) 
        return TRUE;
    else 
        return FALSE;
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
        // We stop in the following conditions:
        // - If the pos encoder says we've reached
        // - If the bl sensor says we've reached
        // If we rotate too far, and still don't see a black line, let's rotate back
        if(
            ((GetShaftCountRight() >= reqd_shaft_counter_int) | (GetShaftCountLeft() >= reqd_shaft_counter_int)) ||
            (IsBlack(CENTER) && (GetShaftCountRight() >= min_shaft_count_int || GetShaftCountLeft() >= min_shaft_count_int))
        ) {
            break;
        }
    }
    PosEncoderStop(); 
    // If we see a black line, then this is the correct place to have stopped
    // Else, we've probably gone too far, and should return back to exactly the degrees that were passed in
    if (IsBlack(CENTER)) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

void RotateBot(int degrees) {
    int too_far = 0;
    if(degrees > 0){
        PosEncoderLeft();
        too_far = AngleRotate(abs(degrees));
        if (too_far == 1) {
            PosEncoderRight();
            PosEncoderAngleRotate(rotation_threshold);
        }
    }
    else if (degrees < 0){
        PosEncoderRight();
        too_far = AngleRotate(abs(degrees));
        if (too_far == 1) {
            PosEncoderLeft();
            PosEncoderAngleRotate(rotation_threshold);
        }
    }
    PosEncoderStop();
}

/*

void MoveBot(int distance_in_mm) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    reqd_shaft_counter = distance_in_mm / 5.338; 
    reqd_shaft_counter_int = (unsigned long int) reqd_shaft_counter;
    ResetRightShaft();
    ResetLeftShaft();

    while(1)
    {
        // LcdPrintf("Right %d %d", (int) GetShaftCountRight(), (int) distance_in_mm);
        if((GetShaftCountRight() > reqd_shaft_counter_int) | (GetShaftCountLeft() > reqd_shaft_counter_int)) {
            // LcdPrintf("Break!");
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
    // LcdPrintf("Passing %d", distance);
    _delay_ms(1000);
    MoveBot(distance);
}

void MoveBotReverse(int distance) {
    PosEncoderBack();
    MoveBot(distance);
}
*/