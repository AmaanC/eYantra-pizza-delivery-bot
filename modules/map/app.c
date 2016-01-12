#include <stdio.h>
#include <stdlib.h>
#include "map.h"

int main() {
    InitGraph();
    Node *start = GetCurrentNode();

    // Just to make sure it worked
    printf("start to r1 cost: %d\n", start->connected[0]->cost);
    printf("r1 to start cost: %d\n", start->connected[0]->ptr->connected[0]->cost);
    printf("start.x: %d\n", start->x);
    printf("r1.x: %d\n", start->connected[0]->ptr->x);
    return 0;
}