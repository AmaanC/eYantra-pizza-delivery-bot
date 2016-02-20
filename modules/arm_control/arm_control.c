#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../servo/servo.h"
//#include "../buzzer/buzzer.h"
#include "arm_control.h"
#include "../bot_info_memory/bot_info_memory.h"
#include "../move_bot_info/move_bot_info.h"
#include "../timeline/timeline.h"
#include <string.h>
#include "../map/map.h"
#include <math.h>

#define TRUE 1
#define FALSE 0

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

BotInfo *bot_info;
bot_info = GetBotInfo();


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
    if(gripper_num == 0) {
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

void LiftPizza(int arm_num) {
    ArmDown(arm_num);
    CloseGripper(arm_num);
    ArmUp(arm_num);
}

void DropPizza(int arm_num) {
    ArmDown(arm_num);
    OpenGripper(arm_num);
    ArmUp(arm_num);
}

//TODO: if pizza is already at H2DA, turn around and deposite it at H2DB
void PickPizzaUp(Pizza *pizza) {
    bot_info = GetBotInfo();
    if(GetGripperStatus(arm1->gripper_servo)) {
        RotateBot(180);
        _delay_ms(500);
        LiftPizza(arm2->gripper_servo);
        bot_info->arm2->carrying = pizza;
    }
    else {
        LiftPizza(arm1->gripper_servo);
        bot_info->arm1->carrying = pizza;
    }
}

Node *GetDepForHouse(Node *house) {
    if(house->name[0] == "H") {
        if(IsPizzaAt(GetNodeByName(strcat(house->name,"DA"))){
            if(IsPizzaAt(GetNodeByName(strcat(house->name,"DB"))) {
                return NULL;
            }
            else {
                return GetNodeByName(strcat(house->name,"DB"));
            }
        }
        else {
            return GetNodeByName(strcat(house->name,"DA"));
        }
    }
    else {
        return NULL;
    }
}

void DepositPizza(Pizza *pizza) {
    Node *deposition_zone, *current_node;
    float deg_to_dep = 0;
    float current_arm_deg = 0;
    current_node = GetCurrentNode();
    Arm *correct_arm;

    deg_to_dep = RadToDeg(atan2(deposition_zone->y - current_node->y, deposition_zone->x - current_node->x));
    if(bot_info->arm1->carrying == pizza) {
        correct_arm = bot_info->arm1;
    }
    else {
        correct_arm = bot_info->arm2;
    }

    // The current arm angle in absolute terms
    current_arm_deg = bot_info->cur_position->cur_deg + correct_arm->angle;

    RotateBot((int) (deg_to_dep - current_arm_deg));
    // DropPizza();
    
    // if(bot_info->arm2->carrying == pizza) {
    //     DropPizza(bot_info->arm2->lever_servo);
    // }
}


