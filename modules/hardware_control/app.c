#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../map/map.h"
#include "../dijkstra/dijkstra.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bot_memory/bot_memory.h"
#include "../lcd/lcd.h"
#include "../hardware_control/hardware_control.h"

int main() {
    InitBotInfo();
    InitGraph();
    // LcdInitDevices();
    // LcdSet4Bit();
    // LcdInit();
    // MoveBotInitDevices();

    MoveBotToNode(GetNodeByName("H5"));
    MoveBotToNode(GetNodeByName("r12"));
    MoveBotToNode(GetNodeByName("r13"));
    MoveBotToNode(GetNodeByName("r12"));

    return 0;
}