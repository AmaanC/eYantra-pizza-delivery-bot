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

// Number of nodes (vertices) in the graph. This *should* be stored in a Graph struct, but
// temporarily, this will do. TODO: REMOVE THIS
// The value is filled in by the DFSEval function
int num_nodes = 0;

// Returns a pointer to a node
Node* CreateNode(int x, int y, int num_connected, char *name) {
    int i;
    Node *new_node;
    new_node = malloc(sizeof(Node));
    new_node->name = malloc(5); // 5 char array for the name

    new_node->x = x;
    new_node->y = y;
    new_node->num_connected = num_connected;
    new_node->name = name;

    new_node->connected = malloc(new_node->num_connected * sizeof(Connection*)); // Create space for an array of connection pointers
    for (i = 0; i < num_connected; i++) {
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

    Node *a, *b, *c, *d, *e, *f, *g, *h;
    // Initialize the 2 nodes with their x, y, and number of connected nodes
    a = CreateNode(0, 0, 2, "1");
    b = CreateNode(0, 0, 1, "2");
    c = CreateNode(0, 0, 3, "3");
    d = CreateNode(0, 0, 3, "4");
    e = CreateNode(0, 0, 1, "8");
    f = CreateNode(0, 0, 3, "9");
    g = CreateNode(0, 0, 1, "11");
    h = CreateNode(0, 0, 2, "13");
    // Actually create the connected between the nodes with the cost at the edge
    ConnectNodes(a, d, 5);
    ConnectNodes(a, h, 4);
    ConnectNodes(b, d, 9);
    ConnectNodes(c, f, 10);
    ConnectNodes(c, g, 2);
    ConnectNodes(c, h, 8);
    ConnectNodes(d, f, 8);
    ConnectNodes(e, f, 7);

    current_node = g; // We're assuming that we'll start there

    MoveBotToNode(b);
    MoveBotToNode(h);
}

Node* GetCurrentNode() {
    return current_node;
}

// Call it like this:
// DFSEval(GetCurrentNode(), GetCurrentNode()->visited, update_dist)
void DFSEval(Node *source_node, int unvisited_value, void fn()) {
    int i;
    fn(source_node);
    // TODO: REMOVE
    num_nodes++;
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

// If a node isn't already in the array, add it. Else, ignore the call
void UpdateNodeInArray(Node **node_costs, int *len, Node* new_node) {
    int i;
    Node *current_node;
    for (i = 0; i < *len; i++) {
        current_node = node_costs[i];
        if (current_node == new_node) {
            return;
        }
    }
    node_costs[*len] = new_node;
    (*len)++;
}

Node* GetLowestUndone(Node **node_costs, int len) {
    int i;
    float lowest_cost = INFINITY;
    Node *lowest, *current_node;
    for (i = 0; i < len; i++) {
        current_node = node_costs[i];
        if (current_node->done == FALSE) {
            // Get lowest from these
            if (current_node->path_cost < lowest_cost) {
                lowest = current_node;
            }
        }
    }
    return lowest;
}

// Use Dijkstra's algorithm to figure out best path and then use
// move_to module to move the bot through all the nodes
// in the route we choose. (move_to uses the pos_encoder and bl_sensor to
// ensure that the bot travels exactly to the point we want it to.)
// For eg. the path fro going from A -> C is A->B->C
// This function will figure that path out and call move_to(B.x, B.y)
// first. Then it'll call move_to(C.x, C.y) when it is done.
// It'll also update the current_node variable, of course.
PathStack* Dijkstra(Node *source_node, Node *target_node) {
    // Refer to map.h for some documentation too, since it explains how some member variables are used
    // For Dijkstra's algorithm, we need the following:
    // A place to store the current minimum distances to nodes (stored on each Node struct)
    // Every Node stores a prev_node, which refers to the node from where we got to the
    // current one (i.e. using the optimal path, because the prev_node will be updated
    // whenever we find a lower cost path to the current node)
    // This way, to find the path from source to target, we can use reverse iteration!

    // Since we use an adjacency list, to get all the vertices, we
    // can use DFS

    // 1) Init all dists to Infinity
    // 2) Update distances from current_node
    // 3) Select next current_node based on the lowest cost found (for a node which hasn't been *done*)
    // 4) Repeat from step 2

    // i is used for a for loop
    // loop_limiter is used to prevent the program from going into an infinite loop in case a non-existent target node
    // was passed as the argument
    int i, loop_limiter;
    int MAX_ITERATIONS = 10000;
    // accum_cost is the cost we have accrued *up to* the current node
    // temp_cost is the accum_cost + the edge cost for a node. Basically, this is used when we find a new
    // route to a certain node. temp_cost is compared with the Node's current path_cost, and if it's lower
    // the Node's path_cost is updated
    int accum_cost = 0;
    int temp_cost = 0;
    // current_node: the node we're looking at right now
    // counter_node: the node, which is a neighbour of current_node, that we're updating the cost for
    Node *current_node, *counter_node;
    // An array used to find the next lowest cost node, required for the next iteration of cost updating
    // This is an array of Node pointers
    Node **node_costs;
    // Length of the array above
    int node_costs_len = 0;
    // Records the actual final path to be taken from source to dest
    PathStack *final_path;

    current_node = source_node;
    DFSEval(source_node, source_node->visited, InitNodesDijkstra);
    node_costs = malloc(num_nodes * sizeof(Node*));
    // The final_path won't always be this long, but we need enough memory in case it is, somehow
    final_path = malloc(sizeof(PathStack));
    final_path->path = malloc(num_nodes * sizeof(Node*));
    final_path->len = 0;

    source_node->path_cost = 0;
    source_node->done = TRUE;
    loop_limiter = 0;
    printf("Target name: %s\n\n", target_node->name);
    while (current_node != target_node) {
        // The accum_cost is the cost from the source_node to the current_node
        // It'll be used to update the costs of all neighbours
        accum_cost = current_node->path_cost;
        // printf("\n\n%s: %f\n\n", current_node->name, current_node->path_cost);
        current_node->done = TRUE;
        // Update path_costs for all neighbouring nodes
        for (i = 0; i < current_node->counter; i++) {
            counter_node = current_node->connected[i]->ptr;
            if (counter_node->done != TRUE) {
                temp_cost = accum_cost + current_node->connected[i]->cost;
                if (temp_cost < counter_node->path_cost) {
                    counter_node->path_cost = temp_cost;
                    // We found a better way to get to counter_node, so we update its prev_node reference
                    counter_node->prev_node = current_node;

                    // Make sure that the node is in the array
                    UpdateNodeInArray(node_costs, &node_costs_len, counter_node);
                }
            }
        }
        printf("cur_node: %s, accum_cost: %d\n", current_node->name, &current_node == &target_node);
        current_node = GetLowestUndone(node_costs, node_costs_len);
        loop_limiter++;
        if (loop_limiter >= MAX_ITERATIONS) {
            printf("Too many iterations!\n");
            break;
        }
    }
    // Dijkstra's is done!
    // Now we can reverse iterate and use the prev_node pointers to find the path the bot should take
    counter_node = target_node;
    final_path->path[final_path->len] = counter_node;
    final_path->len++;
    do {
        // Iterate backwards
        counter_node = counter_node->prev_node;

        final_path->path[final_path->len] = counter_node;
        final_path->len++;
    }
    while (counter_node != source_node);
    // printf("%d", final_path->len);
    return final_path;
}

void MoveBotToNode(Node* target_node) {
    PathStack *final_path;
    int i;

    final_path = Dijkstra(GetCurrentNode(), target_node);
    for (i = final_path->len - 1; i >= 0; i--) {
        printf("%s, ", final_path->path[i]->name);
    }
    printf("\n");

    // Now that we know the path to take, here's how we actually get there
    // To go from A to D
    // cur_pos = A
    // next_dest = B
    // If curve, use custom function, else
    // Rotate towards next_dest (skip if rotation diff < threshold like 5 degs)
    // Move forward dist using pos encoders
    // Repeat for next pair of nodes
}