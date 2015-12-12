#ifndef SEVENSEGMENT_H_
#define SEVENSEGMENT_H_

void seven_segment_pin_config();
void port_init();
void init_devices();
int convert_to_hex(unsigned int num);
void display_num(unsigned int num);

#endif
