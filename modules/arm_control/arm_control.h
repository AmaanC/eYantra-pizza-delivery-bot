#ifndef ARM_CONTORL_H
#define ARM_CONTROL_H

#include "../timeline/timeline.h"
#include "../bot_memory/bot_memory.h"

void InitArms();
void ArmDown(Arm *arm);
void ArmUp(Arm *arm);
void OpenGripper(Arm *arm);
void CloseGripper(Arm *arm);
void LiftPizza(Arm *arm);
void DropPizza(Arm *arm);
void PickPizzaUp(Pizza *pizza);
Node *GetDepForHouse(Node *house);
void DepositPizza(Pizza *pizza);
Arm *GetFreeArm();

#endif
