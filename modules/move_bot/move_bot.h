#ifndef MOVE_BOT_H_
#define MOVE_BOT_H_

void MoveBotInitDevices();
float PidControl(float curve_value, float required_value);
void RotateBot(int degrees, char at_counter);
float ReadSensors();
char IsBlack(int sensor_num);
char AnyBlack();
char CenterBlack();
int AngleRotate(unsigned int degrees, char (*sensor_check)());
void MoveBot(unsigned char left_velocity, unsigned char right_velocity, int distance_in_mm, unsigned char correction_val);
void MoveBotForward(unsigned char left_velocity, unsigned char right_velocity, int distance);
void MoveBotBackward(unsigned char left_velocity, unsigned char right_velocity, int distance);


#endif