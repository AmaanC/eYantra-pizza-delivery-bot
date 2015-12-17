#ifndef MOTOR_H_
#define MOTOR_H_

void motion_pin_config();
void init_ports();
void timer5_init();
void velocity(unsigned char left_motor, unsigned char right_motor);
void motion_set(unsigned char Direction);
void forward();
void back();
void left();
void right();
void stop();
void init_devices();

#endif