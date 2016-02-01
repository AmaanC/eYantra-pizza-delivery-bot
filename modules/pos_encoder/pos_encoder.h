#ifndef POS_ENCODER_H_
#define POS_ENCODER_H_

void pos_encoder_init_devices();
unsigned long int pos_encoder_get_left();
unsigned long int pos_encoder_get_right();

#endif