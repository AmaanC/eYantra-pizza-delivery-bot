#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../servo/servo.h"
//#include "../buzzer/buzzer.h"
#include "arm_control.h"

int main() {
	while(1) {
		ServoInitDevices();
		PickUpPizza(1);
		PickUpPizza(2);
		_delay_ms(5000);
		DepositPizza(1);
		DepositPizza(2);
		_delay_ms(5000);
	}

}