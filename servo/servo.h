#ifndef SERVO_H_
#define SERVO_H_

void servo_servo1_pin_config (void);
void servo_servo2_pin_config (void);
void servo_servo3_pin_config (void);
void servo_port_init(void);
void servo_timer1_init(void);
void servo_init_devices(void);
void servo_servo_1(unsigned char degrees);
void servo_servo_2(unsigned char degrees);
void servo_servo_3(unsigned char degrees);
void servo_servo_1_free (void);
void servo_servo_2_free (void);
void servo_servo_3_free (void);

#endif