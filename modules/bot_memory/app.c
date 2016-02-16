#include <stdio.h>
#include "bot_memory.h"

int main() {
    BotInfo *bot_info;
    
    InitBotInfo();

    bot_info = GetBotInfo();
    printf("Sensor: %f\n", bot_info->sensor_angle);
    return 0;
}