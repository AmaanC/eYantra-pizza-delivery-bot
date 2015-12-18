#ifndef SHARPSENSOR_H_
#define SHARPSENSOR_H_

void sharp_adc_pin_config();
void sharp_port_init();
void sharp_adc_init();
void sharp_init_devices();
unsigned char sharp_ADC_Conversion(unsigned char Ch);
unsigned int sharp_GP2D12_estimation(unsigned char adc_reading);
int sharp_get_block_size(int distance, int default_dist);

#endif
