#ifndef MOVE_BOT_H_
#define MOVE_BOT_H_

void MoveBotInitDevices();
float PidControl(float curve_value, float required_value);
void RotateBot(int degrees);
void MoveBotForward(int distance);
float ReadSensors();
int IsBlack(int sensor_num);
int AngleRotate(unsigned int degrees);
void MoveBot(int distance_in_mm);
void MoveBotReverse(int distance);


#endif