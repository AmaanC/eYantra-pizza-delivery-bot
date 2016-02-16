#ifndef BUZZER_H_
#define BUZZER_H_

void BuzzerPinConfig();
void BuzzerPortInit();
void BuzzerOn();
void BuzzerOff();
void BuzzerInitDevices();
void BuzzerBeep(int ms);

#endif