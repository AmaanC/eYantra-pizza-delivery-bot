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
#include "../timer/timer.h"
#include "../hardware_control/hardware_control.h"
#include "../fake_gcc_fns/fake_gcc_fns.h"

int main() {
    InitTimer();
    InitBotInfo();
    InitGraph();
    LcdInit();
    MoveBotInitDevices();

    printf("Hello world");
    _delay_ms(1000);
    // LcdPrintf("Before %s", GetNodeByName("r3")->name);

    MoveBotToNode(GetNodeByName("c5"));
    PosEncoderRotateBot((int) GetShortestDeg(-90 - (GetBotInfo()->cur_position->cur_deg + GetBotInfo()->sensor_angle)));
    MoveBotToNode(GetNodeByName("c1"));
    PosEncoderRotateBot((int) GetShortestDeg(-90 - (GetBotInfo()->cur_position->cur_deg + GetBotInfo()->sensor_angle)));
    MoveBotToNode(GetNodeByName("H1"));
    MoveBotToNode(GetNodeByName("H8"));
    // RotateBot(90);
    // CurveTowards(GetNodeByName("r6"), GetNodeByName("r7"));

    LcdPrintf("After");
    return 0;
}
