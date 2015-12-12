#include <stdio.h>
#include <stdlib.h>
#include "map.h"

int main() {

    Node *start, *r1;
    // Initialize the 2 nodes with their x, y, and number of connected nodes
    start = CreateNode(0, 0, 1);
    r1 = CreateNode(20, 20, 1);

    // Actually create the connections between the nodes with the cost at the edge
    ConnectNodes(start, r1, 10);
    
    // Just to make sure it worked
    printf("start to r1 cost: %d\n", start->connected[0]->cost);
    printf("r1 to start cost: %d\n", start->connected[0]->ptr->connected[0]->cost);
    printf("start.x: %d\n", start->x);
    printf("r1.x: %d\n", start->connected[0]->ptr->x);
    return 0;
}