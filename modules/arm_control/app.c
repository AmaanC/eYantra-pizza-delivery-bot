#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "arm_control.h"
#include "../servo/servo.h"
//#include "../buzzer/buzzer.h"
#include "../bot_memory/bot_memory.h"
#include "../move_bot/move_bot.h"
#include "../timeline/timeline.h"
#include "../map/map.h"

int main() {
	ServoInitDevices();
    InitBotInfo();
    Pizza *pizza1, *pizza2;
    Order *order1;
    pizza1 = malloc(sizeof(Pizza));
    order1 = malloc(sizeof(Order));
    pizza2 = malloc(sizeof(Pizza));
    pizza1->colour = 'r';
    pizza2->colour = 'b';
    pizza2->size = 'l';
    pizza1->size = 'm';  
    
    order1->delivery_house = GetNodeByName("H12");

    PickPizzaUp(pizza1);
    PickPizzaUp(pizza2);
    DepositPizza(pizza1, order1);
    free(pizza1);
    free(pizza2);
    return 0;
}