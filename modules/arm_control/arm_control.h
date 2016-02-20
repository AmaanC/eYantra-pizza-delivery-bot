#ifndef ARM_CONTORL_H
#define ARM_CONTROL_H

#include "../timeline/timeline.h"

void ArmDown(int arm_num);
void ArmUp(int arm_num);
void OpenGripper(int gripper_num);
void CloseGripper(int gripper_num);
int GetArmStatus(int pos);
int GetGripperStatus(int pos);
void LiftPizza(int arm_num);
void DropPizza(int arm_num);
void DepositPizza(Pizza *pizza);
void PickPizzaUp(Pizza *pizza);

#endif
