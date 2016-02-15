#ifndef BL_SENSOR_H_
#define BL_SENSOR_H_

void bl_sensor_adc_pin_config ();
void bl_sensor_port_init();
void bl_sensor_timer5_init();
void bl_sensor_adc_init();
unsigned char bl_sensor_ADC_Conversion(unsigned char Ch) ;
void bl_sensor_init_devices ();
unsigned char bl_sensor_get(unsigned char sensor_num);

#endif