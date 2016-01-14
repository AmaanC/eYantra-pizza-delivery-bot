#ifndef COLOR_SENSOR_H_
#define COLOR_SENSOR_H_

void color_pin_config();
void color_interrupt_init();
int color_collect_pulses();
int color_get_red();
int color_get_green();
int color_get_blue();
void color_sensor_scaling();
void color_init_devices();
char color_get();

#endif