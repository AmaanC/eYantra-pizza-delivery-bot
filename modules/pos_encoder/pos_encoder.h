#ifndef POS_ENCODER_H_
#define POS_ENCODER_H_

void pos_encoder_motion_pin_config(); 
void left_encoder_pin_config();
void right_encoder_pin_config();
void pos_encoder_port_init();
void left_position_encoder_interrupt_init();
void right_position_encoder_interrupt_init(); 
void pos_encoder_timer5_init();
void pos_encoder_velocity(unsigned char left_motor, unsigned char right_motor);
void pos_encoder_motion_set (unsigned char Direction);
void pos_encoder_forward();
void pos_encoder_back(); 
void pos_encoder_left();
void pos_encoder_right();
void pos_encoder_soft_left(); 
void pos_encoder_soft_right(); 
void pos_encoder_soft_left_2(); 
void pos_encoder_soft_right_2(); 
void pos_encoder_stop();
void pos_encoder_angle_rotate(unsigned int Degrees);
void pos_encoder_linear_distance_mm(unsigned int DistanceInMM);
void pos_encoder_forward_mm(unsigned int DistanceInMM);
void pos_encoder_back_mm(unsigned int DistanceInMM);
void pos_encoder_left_degrees(unsigned int Degrees) ;
void pos_encoder_right_degrees(unsigned int Degrees);
void pos_encoder_soft_left_degrees(unsigned int Degrees);
void pos_encoder_soft_right_degrees(unsigned int Degrees);
void pos_encoder_soft_left_2_degrees(unsigned int Degrees);
void pos_encoder_soft_right_2_degrees(unsigned int Degrees);
void pos_encoder_init_devices();
void pos_encoder_rotate_bot(int Degrees);


#endif