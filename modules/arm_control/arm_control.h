#ifndef ARM_CONTORL_H
#define ARM_CONTROL_H

void ArmDown(int arm_num);
void ArmUp(int arm_num);
void OpenGripper(int gripper_num);
void CloseGripper(int gripper_num);
int GetArmStatus(int pos);
int GetGripperStatus(int pos);
void PickUpPizza(int arm_num);
void DepositPizza(int arm_num);

#endif
