#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../servo/servo.h"
//#include "../buzzer/buzzer.h"
#include "arm_control.h"

const int LEVER_1_UP = 20;
const int LEVER_2_UP = 160;
const int LEVER_1_DOWN = 130;
const int LEVER_2_DOWN = 50;

const int GRIPPER_2_OPEN = 60;
const int GRIPPER_1_OPEN = 125;
const int GRIPPER_2_CLOSE = 120;
const int GRIPPER_1_CLOSE = 95;

int arm_position[2];
int gripper_position[2];

// 1: the whole arm on the right including the gripper mechanism.
// 2: the whole arm on the left including the gripper mechanism.

void ArmDown(int arm_num) {
	if(arm_num == 0) {
		arm_position[0] = 1;
		ServoControl(1, LEVER_1_DOWN); // Lever down
        _delay_ms(1000);
	}

	else {
		arm_position[1] = 1;
		ServoControl(4, LEVER_2_DOWN); // Lever down
		_delay_ms(1000);
	}
}

void ArmUp(int arm_num) {
	if(arm_num == 0) {
		arm_position[0] = 0;
		ServoControl(1, LEVER_1_UP); // Lever up
        _delay_ms(1000);
	}

	else {
		arm_position[1] = 0;
		ServoControl(4, LEVER_2_UP); // Lever up
		_delay_ms(1000);
	}
}

void OpenGripper(int gripper_num) {
	if(gripper_num == 0) {
		gripper_position[0] = 0;
		ServoControl(2, GRIPPER_1_OPEN); // Gripper open
        _delay_ms(1000);
	}
	else {
		gripper_position[1] = 0;
		ServoControl(3, GRIPPER_2_OPEN); // Gripper open
        _delay_ms(1000);
	}
}

void CloseGripper(int gripper_num) {
	if(gripper_num == 1) {
		gripper_position[0] = 1;
		ServoControl(2, GRIPPER_1_CLOSE); // Gripper close
        _delay_ms(3000);
	}
	else {
		gripper_position[1] = 1;
		ServoControl(3, GRIPPER_2_CLOSE); // Gripper close
        _delay_ms(3000);
	}
}

int GetArmStatus(int pos) {
	return arm_position[pos];
}

int GetGripperStatus(int pos) {
	return gripper_position[pos];
}

void PickUpPizza(int arm_num) {
	ArmDown(arm_num);
	CloseGripper(arm_num);
	ArmUp(arm_num);
}

void DepositPizza(int arm_num) {
	ArmDown(arm_num);
	OpenGripper(arm_num);
	ArmUp(arm_num);
}
