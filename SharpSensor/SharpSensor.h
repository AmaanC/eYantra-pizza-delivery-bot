#ifndef SHARPSENSOR_H_
#define SHARPSENSOR_H_

void adc_pin_config();
void port_init();
void adc_init();
void init_devices();
unsigned char ADC_Conversion(unsigned char Ch);
unsigned int Sharp_GP2D12_estimation(unsigned char adc_reading);
int get_block_size(int distance, int default_dist);

#endif
