// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

struct NodeStruct; // Forward decleration, since both structs depend on each other

typedef struct {
    int cost;
    struct NodeStruct *ptr;
} Connection;

// Types:
// 0 is regular black node (unnamed on the flex map)
// 1 is a pizza pick up node
// 2 is a house door node
// 3 is a house deposit node
typedef struct NodeStruct {
    unsigned int type;
    // Co-ordinates are relative to the "start" block
    int x;
    int y;
    // An array of "connection" structures. As seen above, every connection has a cost associated with it (in time)
    // and a pointer.
    // So, for example, node A might be connected to B, C, D.
    // A.connected[0] will be a struct which tells you the cost from A to B, and lets you move to node B.
    Connection **connected;
} Node;

// Now we create the map
// The journey is long and treacherous
// For we must brave many obstacles
// Some of which be tall
// Some be small
// Most be crises about poetic freedom and grammar instincts
// We shall prevail
// Defeat them against all odds

int main() {
    Node Start;
    Start.x = 0;
    Start.y = 0;
    Start.connected = malloc(1 * sizeof(Connection));

    Node R1;
    Start.connected[0] = malloc(sizeof(Connection));
    Start.connected[0]->cost = 10;
    Start.connected[0]->ptr = &R1;

    printf("%d", Start.connected[0]->cost);
    return 0;
}