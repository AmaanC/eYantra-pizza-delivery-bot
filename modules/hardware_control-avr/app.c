#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// #include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../map/map.h"
#include "../dijkstra/dijkstra.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bot_memory/bot_memory.h"
#include "../lcd/lcd.h"
#include "../hardware_control/hardware_control.h"
#include "../fake_gcc_fns/fake_gcc_fns.h"

int main() {
    InitBotInfo();
    InitGraph();
    LcdInit();
    MoveBotInitDevices();

    printf("Hello world");
    _delay_ms(1000);
    // LcdPrintf("Before %s", GetNodeByName("r3")->name);

    MoveBotToNode(GetNodeByName("H4"));
    MoveBotToNode(GetNodeByName("H10"));
    MoveBotToNode(GetNodeByName("S"));
    MoveBotToNode(GetNodeByName("P1"));

    LcdPrintf("After");
    return 0;
}