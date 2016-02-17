#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "custom_delay.h"


void CustomDelay(int ms) {
  while (0 < ms) {  
    _delay_ms(1);
    --ms;
  }
}