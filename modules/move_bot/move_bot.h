#ifndef MOVE_BOT_H_
#define MOVE_BOT_H_

void MoveBotInitDevices();
float PidControl(float curve_value, float required_value);
void RotateBot(int degrees);
float ReadSensors();
int IsBlack(int sensor_num);
int AngleRotate(unsigned int degrees);
void MoveBot(unsigned char left_velocity, unsigned char right_velocity, int distance_in_mm);
void MoveBotForward(unsigned char left_velocity, unsigned char right_velocity, int distance);
void MoveBotReverse(unsigned char left_velocity, unsigned char right_velocity, int distance);


#endif