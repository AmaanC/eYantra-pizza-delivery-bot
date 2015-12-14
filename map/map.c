// Trying to follow Google's style guide (for C++, since they don't have one for C)
// Source: https://google.github.io/styleguide/cppguide.html
// Important to keep in mind: https://google.github.io/styleguide/cppguide.html#General_Naming_Rules

// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "map.h"

// This is how the bot's position is maintained
Node *current_node;

// Returns a pointer to a node
Node* CreateNode(int x, int y, int num_connections) {
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

// Note that this function connects "a" to "b" *and* "b" to "a". They don't need to be connected individually
void ConnectNodes(Node *a, Node *b, int cost) {
    int a_count = a->counter;
    int b_count = b->counter;

    a->connected[a_count]->ptr = b; // a ---> b
    b->connected[b_count]->ptr = a; // a <--> b
    a->connected[a_count]->cost = b->connected[b_count]->cost = cost; // a <---cost---> b

    a->counter++;
    b->counter++;
}

// Now we create the map
// The journey is long and treacherous
// For we must brave many obstacles
// Some of which be tall
// Some be small
// Most be crises about poetic freedom and grammar instincts
// We shall prevail
// Defeat them against all odds
void InitGraph() {

    Node *start, *r1;
    // Initialize the 2 nodes with their x, y, and number of connected nodes
    start = CreateNode(0, 0, 1);
    r1 = CreateNode(20, 20, 1);

    // Actually create the connections between the nodes with the cost at the edge
    ConnectNodes(start, r1, 10);

    current_node = start; // We're assuming that we'll start there
}

Node* GetCurrentNode() {
    return current_node;
}