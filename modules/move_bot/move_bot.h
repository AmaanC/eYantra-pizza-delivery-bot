#ifndef MOVE_BOT_H_
#define MOVE_BOT_H_

void MoveBotInitDevices();
void RotateBot(int degrees);
void MoveBotForwardMm(unsigned int distance);
void MoveBotForward(int distance);
void GetSensorsStatus();
int IsBlack(unsigned char sensor_vlaue);

#endif