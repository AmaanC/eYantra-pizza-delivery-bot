#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"
#include "../map/map.h"

int main() {
    OrderList *timeline;
    int i;

    InitGraph();
    InitTimeline();
    timeline = GetTimeline();
    printf("Num orders: %d\n", timeline->len);
    printf("Order 2 start: %f\n", timeline->orders[2]->block->start);
    FindNextDefiniteNeed(timeline);
    
    for (i = 0; i < timeline->len; i++) {
        Display(timeline->orders[i]);
    }
    return 0;
}