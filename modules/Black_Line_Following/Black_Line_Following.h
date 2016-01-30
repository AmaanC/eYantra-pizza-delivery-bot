#ifndef BLACK_LINE_FOLLOWING_H_
#define BLACK_LINE_FOLLOWING_H_

void black_line_following_lcd_port_config ();
void black_line_following_adc_pin_config ();
void black_line_following_motion_pin_config () ;
void black_line_following_port_init();
void black_line_following_timer5_init();
void black_line_following_adc_init();
unsigned char black_line_following_ADC_Conversion(unsigned char Ch) ;
void black_line_following_print_sensor(char row, char coloumn,unsigned char channel);
void black_line_following_velocity (unsigned char left_motor, unsigned char right_motor);
void black_line_following_motion_set (unsigned char Direction);
void black_line_following_forward () ;
void black_line_following_stop ();
void black_line_following_init_devices ();

#endif