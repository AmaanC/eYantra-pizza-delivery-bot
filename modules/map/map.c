// Trying to follow Google's style guide (for C++, since they don't have one for C)
// Source: https://google.github.io/styleguide/cppguide.html
// Important to keep in mind: https://google.github.io/styleguide/cppguide.html#General_Naming_Rules

#define TRUE 1
#define FALSE 0

// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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

// Call it like this:
// DFSEval(GetCurrentNode(), GetCurrentNode()->visited, update_dist)
void DFSEval(Node *source_node, int unvisited_value, void fn()) {
    int i;
    fn(source_node);
    source_node->visited = !unvisited_value;
    for (i = 0; i < source_node->counter; i++) {
        // If it hasn't been visited already, run DFS on the node too.
        if (source_node->connected[i]->ptr->visited == unvisited_value) {
            DFSEval(source_node->connected[i]->ptr, unvisited_value, fn);
        }
    }
}

// The things you need to do to initialize all the nodes for Dijkstra's algorithm
// Namely, set the path_cost to infinite
// And set the done flag to FALSE
void InitNodesDijkstra(Node* current_node) {
    current_node->path_cost = INFINITY; // INFINITY is a macro from math.h
    current_node->done = FALSE;
}

// Use Dijkstra's algorithm to figure out best path and then use
// move_to module to move the bot through all the nodes
// in the route we choose. (move_to uses the pos_encoder and bl_sensor to
// ensure that the bot travels exactly to the point we want it to.)
// For eg. the path fro going from A -> C is A->B->C
// This function will figure that path out and call move_to(B.x, B.y)
// first. Then it'll call move_to(C.x, C.y) when it is done.
// It'll also update the current_node variable, of course.
void MoveBotToNode(Node* target_node) {
    // For Dijkstra's algorithm, we need the following:
    // An array to store the current minimum distances to nodes
    // An array to store the current optimal path to target

    // Since we use an adjacency list, to get all the vertices, we
    // can use DFS

    // 1) Init all dists to Infinity
    // 2) Update distances from current_node
    // 3) Select next current_node based on the lowest cost found (for a node which hasn't been *done*)
    // 4) Repeat from step 2
    Node *current_node;
    Node *source_node = GetCurrentNode();
    source_node->path_cost = 0;
    source_node->done = TRUE;

    current_node = source_node;
    DFSEval(source_node, source_node->visited, InitNodesDijkstra);
    while (current_node != target_node) {
        // Update path_costs for all neighbouring nodes
    }
}