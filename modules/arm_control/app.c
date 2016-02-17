#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../servo/servo.h"
//#include "../buzzer/buzzer.h"
#include "arm_control.h"

int main() {
	while(1) {
		ServoInitDevices();
		ArmDown(1);
		ArmDown(2);
		CloseGripper(1);
		CloseGripper(2);
		ArmUp(1);
		ArmUp(2);
		_delay_ms(5000);
		ArmDown(1);
		ArmDown(2);
		OpenGripper(1);
		OpenGripper(2);
		ArmUp(1);
		ArmUp(2);
		_delay_ms(5000);
	}

}