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

int main() {
	while(1) {
		ServoInitDevices();
		//PickPizzaUp(1);
		//PickPizzaUp(2);
		_delay_ms(5000);
		//DepositPizza(1);
		//DepositPizza(2);
		_delay_ms(5000);
	}

}