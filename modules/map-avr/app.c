#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "../pos_encoder/pos_encoder.h"
#include "../lcd/lcd.h"

int main() {
    pos_encoder_init_devices();
    pos_encoder_timer5_init();
    lcd_init_devices();
    lcd_set_4bit();
    lcd_init();

    lcd_printf("Starting");
    _delay_ms(1000);
    
    InitGraph();
    // Node *start = GetCurrentNode();
    // DFSEval(start, start->visited, InitNodesDijkstra);
    // MoveBotToNode(start->connected[0]->ptr->connected[1]->ptr);

    // Just to make sure it worked
    // printf("start to r1 cost: %d\n", start->connected[0]->cost);
    // printf("r1 to start cost: %d\n", start->connected[0]->ptr->connected[0]->cost);
    // printf("start.name: %s\n", GetNodeByName("H12")->name);
    // printf("r1.name: %s\n", start->connected[0]->ptr->name);
    // printf("start.x: %d\n", start->x);
    // printf("r1.x: %d\n", start->connected[0]->ptr->x);
    // printf("start.path_cost: %f\n", start->path_cost);
    // printf("r1.path_cost: %f\n", start->connected[0]->ptr->path_cost);
    return 0;
}