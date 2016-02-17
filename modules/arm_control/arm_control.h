#ifndef ARM_CONTORL_H
#define ARM_CONTROL_H

typedef struct ArmPosition {
	//1 : arm is down
	//2 : arm is up
	int current_position;
} ArmPosition;

typedef struct GripperPosition {
	//1: gripper is closed
	//2: gripper is open
	int current_position;
} GripperPosition;

void ArmDown(int arm_num);
void ArmUp(int arm_num);
void OpenGripper(int gripper_num);
void CloseGripper(int gripper_num);
int GetArmStatus(ArmPosition *Arm);
int GetGripperStatus(GripperPosition *Gripper);

#endif
