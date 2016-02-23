#ifndef SHARPSENSOR_H_
#define SHARPSENSOR_H_

void SharpAdcPinConfig();
void SharpPortInit();
void SharpAdcInit();
void SharpInitDevices();
unsigned char SharpAdcConversion(unsigned char Ch);
unsigned int SharpGp2d12Estimation(unsigned char adc_reading);
int SharpgetBlockSize(int distance, int default_dist);
char SharpGetBlockType();

#endif
