#include <util/delay.h>
#include "custom_delay.h"

void custom_delay(int ms)
{
  while (0 < ms)
  {  
    _delay_ms(1);
    --ms;
  }
}