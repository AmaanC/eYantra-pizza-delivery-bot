#ifndef BOT_MEMORY_H_
#define BOT_MEMORY_H_

#include "../timeline/timeline.h"

// A struct to store the position of the bot
typedef struct _Pos {
    Node *cur_node;
    float cur_deg;
} Position;

typedef struct _Arm {
    // Just like sensor_angle for the BotInfo struct, this will indicate where in relation to the bot
    // the arm is
    float angle;

    // The following servo numbers are used to make functions like LowerArm(Arm *arm) and so on
    // Gripper servo number
    int gripper_servo;
    // Lever servo number
    int lever_servo;

    // The order it is carrying (can be NULL)
    Pizza *carrying;
} Arm;

// The bot should have the following info
typedef struct _BotInfo {
    Position *cur_position;
    // If the bot is facing 0 degrees (on the unit circle), sensor_angle is where the sharp and
    // colour sensors would be in relation to the bot
    // We've positioned it on the right side of the bot, so this will be -90deg
    float sensor_angle;
    // 2 arms:
    Arm *arm1;
    Arm *arm2;
} BotInfo;

Arm *CreateArm();
Arm *InitArm();

void InitBotInfo();
BotInfo *GetBotInfo();

#endif
