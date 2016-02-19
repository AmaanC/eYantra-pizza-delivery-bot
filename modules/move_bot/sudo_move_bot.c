/*
 * Function Name: AngleRotate
 * Input : degrees
 * Output : 1 or 0
 * Logic: This function will stop the bot in a range of +- rotation_threshold 
          For ex:
          If you pass 90 degrees, but the line is at 87 degrees, it'll stop there
          If the line is at 92 degrees, it'll stop there
          If you're turning on white entirely, it'll turn 95 degrees, and then turn back 5 degrees
          because it didn't find a black line at all
          To indicate that it's turned too far, we return 1
          Else we return 0
 * Example Call: too_far = AngleRotate(90);
*/

void MoveBotRight() {
    PosEncoderVelocity(high_velocity,low_velocity);
    PosEncoderForward();
}

void MoveBotLeft() {
    PosEncoderVelocity(low_velocity, high_velocity);
    PosEncoderForward();
}

void MoveBotReverseRight() {
    PosEncoderVelocity(high_velocity, low_velocity);
    PosEncoderBack();
}

void MoveBotReverseLeft() {
    PosEncoderVelocity(low_velocity, high_velocity);
    PosEncoderBack();
}

void MoveBotStraight() {
    PosEncoderVelocity(high_velocity, high_velocity);
    PosEncoderForward();
}

void MoveBotReverse() {
    PosEncoderVelocity(high_velocity, high_velocity);
    PosEncoderBack();
}

/*
 * Function Name: GetSensorsStatus
 * Input : none
 * Output : none
 * Logic: this function gets the status of the black line sensors and stores them in their corresponding variables
 * Example Call: GetSensorsStatus();
*/
void GetSensorsStatus() {
    left_black_line = BlSensorAdcConversion(3);
    center_black_line = BlSensorAdcConversion(2);    
    right_black_line = BlSensorAdcConversion(1);

}

/*
 * Function Name: MoveBotInitDevices()
 * Input : none
 * Output : none
 * Logic: this function initializes the appropriate ports and variables required for the functioning of the program
 * Example Call: MoveBotInitDevices();
*/


/*
 * Function Name: RotateBot
 * Input : degrees
 * Output : none
 * Logic: this function take the degrees the bot has to be rotated, +ve if it has to be rotated left and -ve if 
          the bot is to be rotated right. Then it rotates the bot accordingly, co-ordinating with the white line 
          sensors and the position encoders.
 * Example Call: RotateBot(90); rotates the bot 90 degrees to the left.
*/

static int right_turn = 0;
static int left_turn = 0;

/*
 * Function Name: MoveBotForwardMm
 * Input : distance_int_mm
 * Output : none
 * Logic: this function makes the bot go the asked amount of distance constantly coordinating with the white line
          sensors and position encoders so that the bot doesn't go off track.
 * Example Call: MoveBotForwardMm(100); 
*/



void MoveBotForwardMm(unsigned int distance_in_mm) {
    float reqd_shaft_counter = 0;
    unsigned long int reqd_shaft_counter_int = 0;
    reqd_shaft_counter = distance_in_mm / 5.338; 
    reqd_shaft_counter_int = (unsigned long int) reqd_shaft_counter;
    right_turn = 0;
    left_turn = 0;
    ResetRightShaft();
    ResetLeftShaft();
    GetSensorsStatus();
    previous_sensor_value = center_black_line;

    while(1) {

        if((GetShaftCountRight() > reqd_shaft_counter_int) | (GetShaftCountLeft() > reqd_shaft_counter_int)) {
            break;
        }
        else {
            GetSensorsStatus();
            current_sensor_value = MAX(center_black_line, left_black_line, right_black_line);

            if(current_sensor_value - previous_sensor_value < sensor_threshold) {
                MoveBotStraight();
                previous_sensor_value = current_sensor_value;
                //prev_center_value = current_sensor_value;
            }
            else if((IsBlack(left_black_line))   ) {
                MoveBotLeft();
                previous_sensor_value = left_black_line;
            }
            else if((IsBlack(right_black_line)) && (previous_sensor_value - center_black_line > sensor_threshold)) {
                MoveBotRight();
                previous_sensor_value = right_black_line;
            } 
        }
    } 
    PosEncoderStop(); 
}

void MoveBotForward(int distance) {
    PosEncoderForward();
    MoveBotForwardMm(distance);
}
