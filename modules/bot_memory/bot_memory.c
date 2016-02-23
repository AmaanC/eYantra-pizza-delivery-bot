#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bot_memory.h"

// This is how the bot's memory is maintained
BotInfo *our_bot;

void InitBotInfo() {
    our_bot = malloc(sizeof(BotInfo));
    our_bot->cur_position = malloc(sizeof(Position));
    our_bot->arm1 = malloc(sizeof(Arm));
    our_bot->arm2 = malloc(sizeof(Arm));
    
    our_bot->sensor_angle = -90;

    our_bot->arm1->carrying = NULL;
    our_bot->arm1->angle = -90;
    our_bot->arm1->gripper_servo = 2;
    our_bot->arm1->grip_open = 125;
    our_bot->arm1->grip_close = 95;
    our_bot->arm1->lever_servo = 1;
    our_bot->arm1->lever_up = 20;
    our_bot->arm1->lever_down = 130;

    our_bot->arm2->carrying = NULL;
    our_bot->arm2->angle = 90;
    our_bot->arm2->gripper_servo = 3;
    our_bot->arm1->grip_open = 60;
    our_bot->arm1->grip_close = 120;
    our_bot->arm2->lever_servo = 4;
    our_bot->arm1->lever_up = 160;
    our_bot->arm1->lever_down = 50;
}

BotInfo *GetBotInfo() {
    return our_bot;
}