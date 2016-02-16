#ifndef BL_SENSOR_H_
#define BL_SENSOR_H_

void BlSensorAdcPinConfig ();
void BlSensorPortInit();
void BlSensorTimer5_init();
void BlSensorAdcInit();
unsigned char BlSensorAdcConversion(unsigned char Ch) ;
void BlSensorInitDevices ();
unsigned char BlSensorGet(unsigned char sensor_num);

#endif