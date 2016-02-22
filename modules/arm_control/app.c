#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../servo/servo.h"
//#include "../buzzer/buzzer.h"
#include "arm_control.h"
#include "../bot_memory/bot_memory.h"
#include "../move_bot/move_bot.h"
#include "../timeline/timeline.h"
#include <string.h>
#include "../map/map.h"
#include <math.h>
#include <stdlib.h>

int main() {
	ServoInitDevices();
    InitialiseBotInfo();
    Pizza *pizza1, *pizza2;
    pizza1 = malloc(sizeof(Pizza));
    pizza2 = malloc(sizeof(Pizza));
    pizza1->colour = 'r';
    pizza2->colour = 'b';
    pizza2->size = 'l';
    pizza1->size = 'm';  
    PickPizzaUp(pizza1);
    PickPizzaUp(pizza2);
    DepositPizza(pizza1);
    free(pizza1);
    free(pizza2);
    return 0;
}