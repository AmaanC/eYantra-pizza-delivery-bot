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
    // Length of the array called "connected" below
    int num_connections;
    // So we can easily "push" more nodes using the function. Just counts how many connections have been made already
    int counter;

    // An array of "connection" structure pointers. As seen above, every connection has a cost associated with it (in time)
    // and a pointer to a node.
    // So, for example, node A might be connected to B, C, D.
    // A.connected[0] will be a struct which tells you the cost from A to B, and lets you move to node B from A.
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

// Returns a pointer to a node
Node* CreateNode(x, y, num_connections) {
    int i;
    Node *new_node;
    new_node = malloc(sizeof(Node));

    new_node->x = x;
    new_node->y = y;
    new_node->num_connections = num_connections;

    new_node->connected = malloc(new_node->num_connections * sizeof(Connection*)); // Create space for an array of connection pointers
    for (i = 0; i < num_connections; i++) {
        new_node->connected[i] = malloc(sizeof(Connection)); // Create space for every individual connection in the array
    }

    return new_node;
}

void connectNodes(Node *a, Node *b, int cost) {
    int a_count = a->counter;
    int b_count = b->counter;

    a->connected[a_count]->ptr = b; // a ---> b
    b->connected[b_count]->ptr = a; // a <--> b
    a->connected[a_count]->cost = b->connected[b_count]->cost = cost; // a <---cost---> b

    a->counter++;
    b->counter++;
}

int main() {
    Node *start, *r1;
    // Initialize the 2 nodes with their x, y, and number of connected nodes
    start = CreateNode(0, 0, 1);
    r1 = CreateNode(20, 20, 1);

    // Actually create the connections between the nodes with the cost at the edge
    connectNodes(start, r1, 10);

    // Just to make sure it worked
    printf("start to r1 cost: %d\n", start->connected[0]->cost);
    printf("r1 to start cost: %d\n", start->connected[0]->ptr->connected[0]->cost);
    printf("start.x: %d\n", start->x);
    printf("r1.x: %d\n", start->connected[0]->ptr->x);
    return 0;
}