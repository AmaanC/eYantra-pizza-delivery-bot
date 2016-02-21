#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <math.h>
#include <stdlib.h>
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

unsigned char correction_val = 15;
unsigned int degrees; //to accept angle in degrees for turning

int last_left = 0;
int last_center = 0;
int last_right = 0;

const int rotation_threshold = 45; // in degrees

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
    reqd_shaft_counter = (float) (degrees + 2 * rotation_threshold) / 4.090;
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

void MoveBot(unsigned char left_velocity, unsigned char right_velocity, int distance_in_mm) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    reqd_shaft_counter = distance_in_mm / 5.338; 
    reqd_shaft_counter_int = (unsigned long int) reqd_shaft_counter;
    
    int left_corrected = left_velocity;
    int right_corrected = right_velocity;

    ResetRightShaft();
    ResetLeftShaft();

    while (1) {
        // LcdPrintf("Right %d %d", (int) GetShaftCountRight(), (int) distance_in_mm);
        if((GetShaftCountRight() > reqd_shaft_counter_int) | (GetShaftCountLeft() > reqd_shaft_counter_int)) {
            // LcdPrintf("Break!");
            break;
        }

        // if (IsBlack(CENTER)) {
        left_corrected = left_velocity;
        right_corrected = right_velocity;
        // }
        if (IsBlack(LEFT) || last_left) {
            if (0xFF - correction_val < right_corrected) {
                left_corrected -= correction_val;
            }
            else {
                right_corrected += correction_val;
            }
        }
        else if (IsBlack(RIGHT) || last_right) {
            // PosEncoderVelocity(high_velocity, low_velocity);
            // Left velocity would overflow if we added to it
            // So we subtract from right
            if (0xFF - correction_val < left_velocity) {
                right_corrected -= correction_val;
            }
            else {
                left_corrected += correction_val;
            }
        }
        else if (
            (IsBlack(LEFT) && IsBlack(RIGHT)) ||
            (last_left && last_right)
        ) {
            // PosEncoderVelocity(high_velocity, high_velocity);
            left_corrected = left_velocity;
            right_corrected = right_velocity;
        }
        last_left = IsBlack(LEFT);
        last_right = IsBlack(RIGHT);

        if (right_corrected > 0xFF) {
            right_corrected = 0xFF;
        }
        if (left_corrected > 0xFF) {
            left_corrected = 0xFF;
        }

        // TODO: Think about how overflows and underflows screw up our corrections

        PosEncoderVelocity(left_corrected, right_corrected);
    }
    PosEncoderStop();
}

void MoveBotForward(unsigned char left_velocity, unsigned char right_velocity, int distance) {
    PosEncoderForward();
    MoveBot(left_velocity, right_velocity, distance);
}

void MoveBotReverse(int distance) {
    PosEncoderBack();
    // MoveBot(distance);
}
