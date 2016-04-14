#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../map/map.h"
#include "../dijkstra/dijkstra.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bot_memory/bot_memory.h"
#include "../lcd/lcd.h"
#include "../move_bot/move_bot.h"
#include "../hardware_control/hardware_control.h"
#include "../timeline/timeline.h"
#include "../buzzer/buzzer.h"
#include "../servo/servo.h"
#include "../seven_segment/seven_segment.h"
#include "../color_sensor/color_sensor.h"
#include "../sharp_sensor/sharp_sensor.h"
#include "../arm_control/arm_control.h"

void MoveBotToName(char *name) {
    MoveBotToNode(GetNodeByName(name));
}

