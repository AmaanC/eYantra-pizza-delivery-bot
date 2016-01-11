#ifndef SEVENSEGMENT_H_
#define SEVENSEGMENT_H_

void seven_segment_pin_config();
void seven_port_init();
void seven_init_devices();
int seven_convert_to_hex(unsigned char num);
void seven_display_num(int num);

#endif
