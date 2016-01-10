#ifndef BUZZER_H_
#define BUZZER_H_

void buzzer_pin_config();
void buzzer_port_init();
void buzzer_on();
void buzzer_off();
void buzzer_init_devices();
void buzzer_beep(int ms);

#endif