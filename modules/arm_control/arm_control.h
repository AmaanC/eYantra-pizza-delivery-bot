#ifndef ARM_CONTORL_H
#define ARM_CONTROL_H

#include "../timeline/timeline.h"
#include "../bot_memory/bot_memory.h"

void ArmDown(Arm *arm);
void ArmUp(Arm *arm);
void OpenGripper(Arm *arm);
void CloseGripper(Arm *arm);
void LiftPizza(Arm *arm);
void DropPizza(Arm *arm);
//void DepositPizza(Pizza *pizza);
void PickPizzaUp(Pizza *pizza);
Node *GetDepForHouse(Node *house);

#endif
