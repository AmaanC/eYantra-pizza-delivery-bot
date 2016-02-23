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
#include "../timer-gcc/timer.h"

#define TRUE 1
#define FALSE 0

BotInfo *bot_info;
Graph *our_graph;

void InitialiseBotInfo() {
    bot_info = GetBotInfo();
    our_graph = GetGraph();
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

void PickPizzaUp(Pizza *pizza) {
    if(bot_info->arm1->carrying != NULL) {
        RotateBot(180);
        _delay_ms(500);
        LiftPizza(bot_info->arm2);
        bot_info->arm2->carrying = pizza;
    }
    else {
        LiftPizza(bot_info->arm1);
        bot_info->arm1->carrying = pizza;
    }
}

Node *GetDepForHouse(Node *house) {
    Node *dep_a, *dep_b;
    
    if (house->name[0] != 'H') {
        // It's not a house, so it has no deposition zone
        return NULL;
    }

    dep_a = GetNodeByName(strcat(house->name, "DA");
    dep_b = GetNodeByName(strcat(house->name, "DB");

    if (IsPizzaAt(dep_a, TRUE)) {
        if (IsPizzaAt(dep_b, TRUE)) {
            printf("Too much pizza is bad for you");
            return NULL;
        }
        else {
            return dep_b;
        }
    }
    else {
        return dep_a;
    }
}

void DepositPizza(Pizza *pizza, Order *order) {
    Node *deposition_zone, *current_node;
    float deg_to_dep = 0;
    float current_arm_deg = 0;
    Arm *correct_arm;
    current_node = GetCurrentNode();
    deposition_zone = GetDepForHouse(order->delivery_house);

    deg_to_dep = RadToDeg(atan2(deposition_zone->y - current_node->y, deposition_zone->x - current_node->x));
    if(bot_info->arm1->carrying == pizza) {
        correct_arm = bot_info->arm1;
    }
    else {
        correct_arm = bot_info->arm2;
    }
    // The current arm angle in absolute terms
    current_arm_deg = bot_info->cur_position->cur_deg + correct_arm->angle;

    RotateBot((int) GetShortestDeg(deg_to_dep - current_arm_deg));

    DropPizza(correct_arm);
    pizza->dep_loc = deposition_zone;
    correct_arm->carrying = NULL;
}
