#ifndef POS_ENCODER_H_
#define POS_ENCODER_H_

void motion_pin_config();
void left_encoder_pin_config();
void right_encoder_pin_config();
void port_init();
void left_position_encoder_interrupt_init();
void right_position_encoder_interrupt_init();
void motion_set(unsigned char Direction);
void forward();
void back();
void left();
void right();
void soft_left();
void soft_right();
void soft_left_2();
void soft_right_2();
void stop();
void angle_rotate(unsigned int Degrees);
void linear_distance_mm(unsigned int DistanceInMM);
void forward_mm(unsigned int DistanceInMM);
void back_mm(unsigned int DistanceInMM);
void left_degrees(unsigned int Degrees) ;
void right_degrees(unsigned int Degrees);
void soft_left_degrees(unsigned int Degrees);
void soft_right_degrees(unsigned int Degrees);
void soft_left_2_degrees(unsigned int Degrees);
void soft_right_2_degrees(unsigned int Degrees);
void init_devices();

#endif