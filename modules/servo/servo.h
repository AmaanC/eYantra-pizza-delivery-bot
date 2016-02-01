#ifndef SERVO_H_
#define SERVO_H_

void servo_1_pin_config();
void servo_2_pin_config();
void servo_3_pin_config();
void servo_4_pin_config();

void servo_port_init();
void servo_timer_init();
void servo_init_devices();

void servo_1_to(unsigned char degrees);
void servo_2_to(unsigned char degrees);
void servo_3_to(unsigned char degrees);
void servo_4_to(unsigned char degrees);

void servo_1_free();
void servo_2_free();
void servo_3_free();
void servo_4_free();

#endif