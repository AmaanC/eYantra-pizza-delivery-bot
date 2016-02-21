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
    our_bot->arm2->carrying = NULL;
    our_bot->arm1->angle = -90;
    our_bot->arm2->angle = 90;
    our_bot->arm1->gripper_servo = 0;
    our_bot->arm2->gripper_servo = 1;
    our_bot->arm1->lever_servo = 0;
    our_bot->arm2->lever_servo = 1;
}

BotInfo *GetBotInfo() {
    return our_bot;
}