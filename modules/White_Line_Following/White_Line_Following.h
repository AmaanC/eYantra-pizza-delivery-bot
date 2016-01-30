#ifndef WHITE_LINE_FOLLOWING_H_
#define WHITE_LINE_FOLLOWING_H_

void white_line_following_lcd_port_config ();
void white_line_following_adc_pin_config ();
void white_line_following_motion_pin_config () ;
void white_line_following_port_init();
void white_line_following_timer5_init();
void white_line_following_adc_init();
unsigned char white_line_following_ADC_Conversion(unsigned char Ch) ;
void white_line_following_print_sensor(char row, char coloumn,unsigned char channel);
void white_line_following_velocity (unsigned char left_motor, unsigned char right_motor);
void white_line_following_motion_set (unsigned char Direction);
void white_line_following_forward () ;
void white_line_following_stop ();
void white_line_following_init_devices ();

#endif