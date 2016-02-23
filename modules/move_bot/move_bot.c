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

unsigned char correction_val_forward = 120;
unsigned char correction_val_backward = 60;
unsigned int degrees; //to accept angle in degrees for turning

int last_left = 0;
int last_center = 0;
int last_right = 0;

const int rotation_threshold = 30; // in degrees
const int dist_threshold = 30;
const int distance_to_fix = 80;

void MoveBotInitDevices() {
    BlSensorInitDevices();
    PosEncoderInitDevices();
    PosEncoderTimer5Init();
}

char IsBlack(int sensor_num) {
    if (BlSensorAdcConversion(sensor_num) > 0x28) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

char AnyBlack() {
    return IsBlack(LEFT) || IsBlack(CENTER) || IsBlack(RIGHT);
}

char CenterBlack() {
    return IsBlack(CENTER);
}

int AngleRotate(unsigned int degrees, char (*sensor_check)()) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    float min_shaft_count = 0;
    unsigned long int min_shaft_count_int = 0;
    reqd_shaft_counter = (float) (degrees + rotation_threshold) / 4.090;
    reqd_shaft_counter_int = (unsigned int) reqd_shaft_counter;
    char reached_black = FALSE;
  
    min_shaft_count = (float) (degrees - 0.5 * rotation_threshold) / 4.090;
    min_shaft_count_int = (unsigned int) min_shaft_count;
    ResetLeftShaft(); 
    ResetRightShaft(); 
    // 2 situations
    // We've gone too far ahead
    // We've stopped too short
    // In the first, we want to try rotating extra to find the black line
    // In the second, we want to move the bot a little ahead, and try the same thing
    while (1) {
        if (
            (sensor_check()) &&
            ((GetShaftCountLeft() > min_shaft_count_int) | (GetShaftCountRight() > min_shaft_count_int))
        ) {
            reached_black = TRUE;
            break;
        }

        // We haven't seen a black line yet, so we want to turn 
        if ((GetShaftCountLeft() > reqd_shaft_counter_int) | (GetShaftCountRight() > reqd_shaft_counter_int)) {
            reached_black = FALSE;
            break;
        }
    }
    PosEncoderStop();
    return reached_black;
}

void RotateBot(int degrees, char at_counter) {
    int reached_black = FALSE;
    char (*sensor_check)();
    sensor_check = AnyBlack;
    if (at_counter) {
        sensor_check = CenterBlack;
    }
    if(degrees > 0){
        PosEncoderLeft();
        reached_black = AngleRotate(abs(degrees), sensor_check);
        while (reached_black != TRUE) {
            PosEncoderRotateBot(-degrees - rotation_threshold);
            MoveBotForward(0xFF, 0xFF, distance_to_fix);
            PosEncoderLeft();
            reached_black = AngleRotate(abs(degrees), sensor_check);
        }
    }
    else if (degrees < 0){
        PosEncoderRight();
        reached_black = AngleRotate(abs(degrees), sensor_check);
        while (reached_black != TRUE) {
            PosEncoderRotateBot(degrees + rotation_threshold);
            MoveBotForward(0xFF, 0xFF, distance_to_fix);
            PosEncoderRight();
            reached_black = AngleRotate(abs(degrees), sensor_check);
        }
    }
    PosEncoderStop();
}

void MoveBot(unsigned char left_velocity, unsigned char right_velocity, int distance_in_mm, unsigned char correction_val) {
    float reqd_shaft_counter = 0;
    float min_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    unsigned long int min_shaft_counter_int = 0;
    reqd_shaft_counter = distance_in_mm / 5.338; 
    min_shaft_counter = (distance_in_mm - dist_threshold) / 5.338; 
    reqd_shaft_counter_int = (unsigned long int) reqd_shaft_counter;
    min_shaft_counter_int = (unsigned long int) min_shaft_counter;
    
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

        if (
            IsBlack(CENTER) &&
            (IsBlack(LEFT) || IsBlack(RIGHT)) &&
            ((GetShaftCountRight() > min_shaft_counter_int) | (GetShaftCountLeft() > min_shaft_counter_int))
        ) {
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
    MoveBot(left_velocity, right_velocity, distance, correction_val_forward);
}

void MoveBotBackward(unsigned char left_velocity, unsigned char right_velocity, int distance) {
    PosEncoderBack();
    MoveBot(left_velocity, right_velocity, distance, correction_val_backward);
}
