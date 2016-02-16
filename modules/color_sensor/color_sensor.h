#ifndef COLOR_SENSOR_H_
#define COLOR_SENSOR_H_

void ColorPinConfig();
void ColorInterruptInit();
int ColorCollectPulses();
int ColorGetRed();
int ColorGetGreen();
int ColorGetBlue();
void ColorSensorScaling();
void ColorInitDevices();
char ColorGet();

#endif