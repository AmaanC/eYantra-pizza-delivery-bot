#ifndef MOTOR_H_
#define MOTOR_H_

void MotionPinConfig();
void InitPorts();
void Timer5Init();
void Velocity(unsigned char left_motor, unsigned char right_motor);
void MotionSet(unsigned char Direction);
void Forward();
void Back();
void Left();
void Right();
void Stop();
void InitDevices();

#endif