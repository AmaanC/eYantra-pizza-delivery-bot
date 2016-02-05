#include <stdio.h>
#include <stdlib.h>
#include "map.h"

int main() {
    InitGraph();
    Node *start = GetCurrentNode();
    DFSEval(start, start->visited, InitNodesDijkstra);

    // Just to make sure it worked
    printf("start to r1 cost: %d\n", start->connected[0]->cost);
    printf("r1 to start cost: %d\n", start->connected[0]->ptr->connected[0]->cost);
    printf("start.x: %d\n", start->x);
    printf("r1.x: %d\n", start->connected[0]->ptr->x);
    printf("start.path_cost: %d\n", start->path_cost);
    printf("r1.path_cost: %d\n", start->connected[0]->ptr->path_cost);
    return 0;
}