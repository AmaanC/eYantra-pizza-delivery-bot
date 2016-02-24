#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../servo/servo.h"
//#include "../buzzer/buzzer.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "arm_control.h"
#include "../bot_memory/bot_memory.h"
#include "../move_bot/move_bot.h"
#include "../timeline/timeline.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bl_sensor/bl_sensor.h"
#include "../map/map.h"

#define TRUE 1
#define FALSE 0

BotInfo *bot_info;
Graph *our_graph;

void InitArms() {
    bot_info = GetBotInfo();
    our_graph = GetGraph();
    ServoInitDevices();
    Servo1To(40);
    Servo2To(60);
    Servo3To(130);
    Servo4To(160);
    // ArmUp(bot_info->arm1);
    // ArmUp(bot_info->arm2);
    // OpenGripper(bot_info->arm1);
    // OpenGripper(bot_info->arm2);
}

void ArmDown(Arm *arm) {
    ServoControl(arm->lever_servo, arm->lever_down); // Lever down
    _delay_ms(100);
}

void ArmUp(Arm *arm) {
    ServoControl(arm->lever_servo, arm->lever_up); // Lever up
    _delay_ms(100);
}

void OpenGripper(Arm *arm) {
    ServoControl(arm->gripper_servo, arm->grip_open); // Gripper open
    _delay_ms(100);
}

void CloseGripper(Arm *arm) {
    ServoControl(arm->gripper_servo, arm->grip_close); // Gripper close
    _delay_ms(100);
}

void LiftPizza(Arm *arm) {
    ArmDown(arm);
    CloseGripper(arm);
    ArmUp(arm);
}

void DropPizza(Arm *arm) {
    ArmDown(arm);
    OpenGripper(arm);
    ArmUp(arm);
}

Arm *GetFreeArm() {
    if (bot_info->arm1->carrying == NULL) {
        return bot_info->arm1;
    }
    else if (bot_info->arm2->carrying == NULL) {
        return bot_info->arm2;
    }
    else {
        return NULL;
    }
}

void PickPizzaUp(Pizza *pizza) {
    if (bot_info->arm1->carrying != NULL) {
        LiftPizza(bot_info->arm2);
        bot_info->arm2->carrying = pizza;
    }
    else {
        LiftPizza(bot_info->arm1);
        bot_info->arm1->carrying = pizza;
    }
}

void DepositPizza(Pizza *pizza) {
    Arm *correct_arm;
    if(bot_info->arm1->carrying == pizza) {
        correct_arm = bot_info->arm1;
    }
    else {
        correct_arm = bot_info->arm2;
    }

    DropPizza(correct_arm);
    correct_arm->carrying = NULL;
}
