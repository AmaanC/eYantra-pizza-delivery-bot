#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../servo/servo.h"
#include "../buzzer/buzzer.h"
#include "arm_control.h"

const int LEVER_1_UP = 20;
const int LEVER_2_UP = 160;
const int LEVER_1_DOWN = 130;
const int LEVER_2_DOWN = 50;

const int GRIPPER_2_OPEN = 60;
const int GRIPPER_1_OPEN = 125;
const int GRIPPER_2_CLOSE = 120;
const int GRIPPER_1_CLOSE = 95;

// 1: the whole arm on the right including the gripper mechanism.
// 2: the whole arm on the left including the gripper mechanism.

ArmPosition *Arm1Pos, *Arm2Pos;
GripperPosition *Gripper1Pos, *Gripper2Pos;

void ArmDown(int arm_num) {
	if(arm_num == 1) {
		Arm1Pos->current_position = 1;
		ServoControl(1, LEVER_1_DOWN); // Lever down
        _delay_ms(1000);
	}

	else {
		Arm2Pos->current_position = 1;
		ServoControl(4, LEVER_2_DOWN); // Lever down
		_delay_ms(1000);
	}
}

void ArmUp(int arm_num) {
	if(arm_num == 1) {
		Arm1Pos->current_position = 0;
		ServoControl(1, LEVER_1_UP); // Lever up
        _delay_ms(1000);
	}

	else {
		Arm2Pos->current_position = 0;
		ServoControl(4, LEVER_2_UP); // Lever up
		_delay_ms(1000);
	}
}

void OpenGripper(int gripper_num) {
	if(gripper_num == 1) {
		Gripper1Pos->current_position = 1;
		ServoControl(2, GRIPPER_1_OPEN); // Gripper open
        _delay_ms(1000);
	}
	else {
		Gripper2Pos->current_position = 1;
		ServoControl(3, GRIPPER_2_OPEN); // Gripper open
        _delay_ms(1000);
	}
}

void CloseGripper(int gripper_num) {
	if(gripper_num == 1) {
		Gripper1Pos->current_position = 0;
		ServoControl(2, GRIPPER_1_CLOSE); // Gripper close
        _delay_ms(3000);
	}
	else {
		Gripper2Pos->current_position = 0;
		ServoControl(3, GRIPPER_2_CLOSE); // Gripper close
        _delay_ms(3000);
	}
}

int GetArmStatus(ArmPosition *Arm) {
	return Arm->current_position;
}

int GetGripperStatus(GripperPosition *Gripper) {
	return Gripper->current_position;
}