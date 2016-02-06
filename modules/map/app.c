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
    printf("start.name: %s\n", start->name);
    printf("r1.name: %s\n", start->connected[0]->ptr->name);
    printf("start.x: %d\n", start->x);
    printf("r1.x: %d\n", start->connected[0]->ptr->x);
    printf("start.path_cost: %f\n", start->path_cost);
    printf("r1.path_cost: %f\n", start->connected[0]->ptr->path_cost);
    return 0;
}