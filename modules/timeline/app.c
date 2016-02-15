#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"
#include "../map/map.h"

int main() {
    InitGraph();
    InitTimeline();
    printf("Num orders: %d\n", GetTimeline()->len);
    printf("Order 2 start: %f\n", GetTimeline()->orders[2]->block->start);
    FindNextDefiniteNeed(GetTimeline());

    return 0;
}