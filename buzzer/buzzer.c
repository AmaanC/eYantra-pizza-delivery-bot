// #define __OPTIMIZE__ -O0
// #define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "buzzer.h"

//Function to initialize Buzzer 
void buzzer_pin_config (void)
{
 DDRC = DDRC | 0x08;		//Setting PORTC 3 as output
 PORTC = PORTC & 0xF7;		//Setting PORTC 3 logic low to turnoff buzzer
}

void buzzer_port_init (void)
{
 buzzer_pin_config();
}

void buzzer_on (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore | 0x08;
 PORTC = port_restore;
}

void buzzer_off (void)
{
 unsigned char port_restore = 0;
 port_restore = PINC;
 port_restore = port_restore & 0xF7;
 PORTC = port_restore;
}

void my_delay_ms(int ms)
{
  while (0 < ms)
  {  
    _delay_ms(1);
    --ms;
  }
}

void buzzer_beep(int ms) {
    buzzer_on();
    my_delay_ms(ms);
    buzzer_off();
    my_delay_ms(ms);
}

void buzzer_init_devices (void)
{
 cli(); //Clears the global interrupts
 buzzer_port_init();
 sei(); //Enables the global interrupts
}


