#ifndef SERVO_H_
#define SERVO_H_

void Servo1PinConfig();
void Servo2PinConfig();
void Servo3PinConfig();
void Servo4PinConfig();

void ServoPortInit();
void ServoTimerInit();
void ServoInitDevices();

void Servo1To(unsigned char degrees);
void Servo2To(unsigned char degrees);
void Servo3To(unsigned char degrees);
void Servo4To(unsigned char degrees);

void Servo1Free();
void Servo2Free();
void Servo3Free();
void Servo4Free();

void ServoControl(int servo_num, unsigned char degrees);

#endif