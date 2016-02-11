// Trying to follow Google's style guide (for C++, since they don't have one for C)
// Source: https://google.github.io/styleguide/cppguide.html
// Important to keep in mind: https://google.github.io/styleguide/cppguide.html#General_Naming_Rules

#define TRUE 1
#define FALSE 0
#define PI 3.14159265

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "map.h"
#include "../pos_encoder/pos_encoder.h"

// This is how the bot's (actual) position is maintained
Position *bot_position;

// Number of nodes (vertices) in the graph. This *should* be stored in a Graph struct, but
// temporarily, this will do. TODO: REMOVE THIS
// The value is filled in by the DFSEval function
int num_nodes = 0;

Node **curve_nodes;
const int curve_nodes_len = 8;

// Returns a pointer to a node
Node *CreateNode(int x, int y, int num_connected, char *name) {
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
void ConnectNodes(Node *a, Node *b, float cost) {
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
    Node *H12, *P1, *P2, *P3, *P4, *P5, *P6, *P7, *P8, *P9, *P10, *c1, *c2, *c3,
         *c4, *c5, *c6, *c7, *c8, *c9, *c10, *S, *r1, *r2, *r3, *r4, *r5, *r6, *r7,
         *r8, *r9, *r10, *r11, *r12, *r13, *r14, *r15, *r16, *r17, *r18, *H1DA, *H1DB,
         *H2DA, *H2DB, *H3DA, *H3DB, *H4DA, *H4DB, *H5DA, *H5DB, *H6DA, *H6DB, *H7DA,
         *H7DB, *H8DA, *H8DB, *H1, *H2, *H3, *H4, *H5, *H6, *H7, *H8, *H9, *H10, *H11;

    bot_position = malloc(sizeof(Position));
    curve_nodes = malloc(8 * sizeof(Node*));
    // Initialize the 2 nodes with their x, y, and number of connected nodes

    P1 = CreateNode(-90, 7, 1, "P1");
    P2 = CreateNode(-72, 7, 1, "P2");
    P3 = CreateNode(-54, 7, 1, "P3");
    P4 = CreateNode(-36, 7, 1, "P4");
    P5 = CreateNode(-18, 7, 1, "P5");
    P6 = CreateNode(18, 7, 1, "P6");
    P7 = CreateNode(36, 7, 1, "P7");
    P8 = CreateNode(54, 7, 1, "P8");
    P9 = CreateNode(72, 7, 1, "P9");
    P10 = CreateNode(90, 7, 1, "P10");
    c1 = CreateNode(-90, 25, 2, "c1");
    c2 = CreateNode(-72, 25, 3, "c2");
    c3 = CreateNode(-54, 25, 3, "c3");
    c4 = CreateNode(-36, 25, 3, "c4");
    c5 = CreateNode(-18, 25, 3, "c5");
    c6 = CreateNode(18, 25, 3, "c6");
    c7 = CreateNode(36, 25, 3, "c7");
    c8 = CreateNode(54, 25, 3, "c8");
    c9 = CreateNode(72, 25, 3, "c9");
    c10 = CreateNode(90, 25, 2, "c10");
    S = CreateNode(0, 0, 1, "S");
    r1 = CreateNode(0, 25, 4, "r1");
    r2 = CreateNode(0, 65, 4, "r2");
    r3 = CreateNode(-35, 65, 3, "r3");
    r4 = CreateNode(-67.5, 97.5, 3, "r4");
    r5 = CreateNode(-67.5, 132.5, 3, "r5");
    r6 = CreateNode(-67.5, 167.5, 3, "r6");
    r7 = CreateNode(-35, 200, 3, "r7");
    r8 = CreateNode(0, 200, 3, "r8");
    r9 = CreateNode(35, 200, 3, "r9");
    r10 = CreateNode(67.5, 167.5, 3, "r10");
    r11 = CreateNode(67.5, 132.5, 3, "r11");
    r12 = CreateNode(67.5, 97.5, 3, "r12");
    r13 = CreateNode(35, 65, 3, "r13");
    r14 = CreateNode(0, 102.5, 3, "r14");
    r15 = CreateNode(-30, 132.5, 3, "r15");
    r16 = CreateNode(0, 162.5, 3, "r16");
    r17 = CreateNode(30, 132.5, 3, "r17");
    r18 = CreateNode(0, 132.5, 4, "r18");
    H1DA = CreateNode(-53, 45, 0, "H1DA");
    H1DB = CreateNode(-17, 45, 0, "H1DB");
    H2DA = CreateNode(-87.5, 115.5, 0, "H2DA");
    H2DB = CreateNode(-87.5, 79.5, 0, "H2DB");
    H3DA = CreateNode(-87.5, 185.5, 0, "H3DA");
    H3DB = CreateNode(-87.5, 149.5, 0, "H3DB");
    H4DA = CreateNode(-17, 220, 0, "H4DA");
    H4DB = CreateNode(-53, 220, 0, "H4DB");
    H5DA = CreateNode(53, 220, 0, "H5DA");
    H5DB = CreateNode(17, 220, 0, "H5DB");
    H6DA = CreateNode(87.5, 149.5, 0, "H6DA");
    H6DB = CreateNode(87.5, 185.5, 0, "H6DB");
    H7DA = CreateNode(87.5, 79.5, 0, "H7DA");
    H7DB = CreateNode(87.5, 115.5, 0, "H7DB");
    H8DA = CreateNode(17, 45, 0, "H8DA");
    H8DB = CreateNode(53, 45, 0, "H8DB");
    H1 = CreateNode(-35, 45, 1, "H1");
    H2 = CreateNode(-87.5, 97.5, 1, "H2");
    H3 = CreateNode(-87.5, 167.5, 1, "H3");
    H4 = CreateNode(-35, 220, 1, "H4");
    H5 = CreateNode(35, 220, 1, "H5");
    H6 = CreateNode(87.5, 167.5, 1, "H6");
    H7 = CreateNode(87.5, 97.5, 1, "H7");
    H8 = CreateNode(35, 45, 1, "H8");
    H9 = CreateNode(-30, 102.5, 3, "H9");
    H10 = CreateNode(-30, 162.5, 3, "H10");
    H11 = CreateNode(30, 162.5, 3, "H11");
    H12 = CreateNode(30, 102.5, 3, "H12");
    // Actually create the connected between the nodes with the cost at the edge
    ConnectNodes(S, r1, 1.59);
    ConnectNodes(r1, c5, 1.15);
    ConnectNodes(c5, P5, 1.15);
    ConnectNodes(c5, c4, 1.15);
    ConnectNodes(c4, P4, 1.15);
    ConnectNodes(c4, c3, 1.15);
    ConnectNodes(c3, P3, 1.15);
    ConnectNodes(c3, c2, 1.15);
    ConnectNodes(c2, P2, 1.15);
    ConnectNodes(c2, c1, 1.15);
    ConnectNodes(c1, P1, 1.15);
    ConnectNodes(r1, c6, 1.15);
    ConnectNodes(c6, P6, 1.15);
    ConnectNodes(c6, c7, 1.15);
    ConnectNodes(c7, P7, 1.15);
    ConnectNodes(c7, c8, 1.15);
    ConnectNodes(c8, P8, 1.15);
    ConnectNodes(c8, c9, 1.15);
    ConnectNodes(c9, P9, 1.15);
    ConnectNodes(c9, c10, 1.15);
    ConnectNodes(c10, P10, 1.15);
    ConnectNodes(r1, r2, 2.55);
    ConnectNodes(r2, r3, 2.23);
    ConnectNodes(r3, H1, 1.27);
    ConnectNodes(r3, r4, 3.78);
    ConnectNodes(r4, H2, 1.27);
    ConnectNodes(r4, r5, 2.23);
    ConnectNodes(r5, r15, 2.39);
    ConnectNodes(r5, r6, 2.23);
    ConnectNodes(r6, H3, 1.27);
    ConnectNodes(r6, r7, 3.78);
    ConnectNodes(r7, H4, 1.27);
    ConnectNodes(r7, r8, 2.23);
    ConnectNodes(r8, r16, 2.39);
    ConnectNodes(r8, r9, 2.23);
    ConnectNodes(r9, H5, 1.27);
    ConnectNodes(r9, r10, 3.78);
    ConnectNodes(r10, H6, 1.27);
    ConnectNodes(r10, r11, 2.23);
    ConnectNodes(r11, r17, 2.39);
    ConnectNodes(r11, r12, 2.23);
    ConnectNodes(r12, H7, 1.27);
    ConnectNodes(r12, r13, 3.78);
    ConnectNodes(r13, H8, 1.27);
    ConnectNodes(r13, r2, 2.23);
    ConnectNodes(r2, r14, 2.39);
    ConnectNodes(r14, H9, 1.91);
    ConnectNodes(H9, r15, 1.91);
    ConnectNodes(r15, H10, 1.91);
    ConnectNodes(H10, r16, 1.91);
    ConnectNodes(r16, H11, 1.91);
    ConnectNodes(H11, r17, 1.91);
    ConnectNodes(r17, H12, 1.91);
    ConnectNodes(H12, r14, 1.91);
    ConnectNodes(H9, r18, 2.70);
    ConnectNodes(H10, r18, 2.70);
    ConnectNodes(H11, r18, 2.70);
    ConnectNodes(H12, r18, 2.70);

    // We always start at S
    bot_position->cur_node = S;

    // Define the nodes where we want to use our custom curve function instead of
    // rotating towards the next node and going forward
    // Which direction we're curving in is done by using atan2 and finding the 
    // angle between the nodes
    curve_nodes[0] = r3;
    curve_nodes[1] = r4;
    curve_nodes[2] = r6;
    curve_nodes[3] = r7;
    curve_nodes[4] = r9;
    curve_nodes[5] = r10;
    curve_nodes[6] = r12;
    curve_nodes[7] = r13;

    MoveBotToNode(H12);
}

Node *GetCurrentNode() {
    return bot_position->cur_node;
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
void InitNodesDijkstra(Node *current_node) {
    current_node->path_cost = INFINITY; // INFINITY is a macro from math.h
    current_node->done = FALSE;
}

int IndexOfNode(Node **node_arr, int len, Node *needle) {
    int index = -1;
    int i;
    for (i = 0; i < len; i++) {
        if (node_arr[i] == needle) {
            index = i;
            break;
        }
    }
    return index;
}

// If a node isn't already in the array, add it. Else, ignore the call
void UpdateNodeInArray(Node **node_costs, int *len, Node *new_node) {
    int index;
    Node *current_node;
    index = IndexOfNode(node_costs, *len, new_node);
    // If it is in the array already, ignore the call
    if (index != -1) {
        return;
    }
    // printf("\tPushed new_node %s, %f, %d\n", new_node->name, new_node->path_cost, new_node->done);
    node_costs[*len] = new_node;
    (*len)++;
}

Node *GetLowestUndone(Node **node_costs, int len) {
    int i;
    float lowest_cost = INFINITY;
    Node *lowest, *current_node;
    for (i = 0; i < len; i++) {
        current_node = node_costs[i];
        if (current_node->done == FALSE) {
            // Get lowest from these
            if (current_node->path_cost < lowest_cost) {
                lowest = current_node;
                lowest_cost = current_node->path_cost;
            }
        }
    }
    // printf("\t\tLowest node is: %s, %d\n", lowest->name, len);
    return lowest;
}

// Time taken for the bot to turn X radians. Needs to factor into Dijkstra's
float GetRotationCost(float radians) {
    // TODO: Use actual measured value
    return 0.5 * fabs(radians);
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
    // rotation_cost is the cost of rotating from node A to node B
    // rot_radians is the new enter_radians for node B
    // temp_radians is the angle the bot will start from at node A
    float accum_cost = 0;
    float temp_cost = 0;
    float rotation_cost = 0;
    float rot_radians = 0;
    float temp_radians = 0;
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
    final_path->top = 0;

    source_node->path_cost = 0;
    source_node->done = TRUE;
    source_node->enter_radians = bot_position->cur_radians;
    loop_limiter = 0;
    printf("Target name: %s\n\n", target_node->name);
    while (current_node != target_node) {
        // The accum_cost is the cost from the source_node to the current_node
        // It'll be used to update the costs of all neighbours
        // accum_cost = current_node->path_cost;
        // printf("\n\n%s: %f\n\n", current_node->name, current_node->path_cost);
        current_node->done = TRUE;
        // Update path_costs for all neighbouring nodes
        for (i = 0; i < current_node->counter; i++) {
            counter_node = current_node->connected[i]->ptr;
            accum_cost = current_node->path_cost;

            if (counter_node->done != TRUE) {
                temp_radians = current_node->enter_radians;
                // the angle between the current node & the counter node
                // atan2 always returns in the range of -pi to pi, so we don't need to worry about
                // sanitizing its output
                rot_radians = atan2(counter_node->y - current_node->y, counter_node->x - current_node->x);
                rotation_cost = GetRotationCost(temp_radians - rot_radians);
                temp_cost = accum_cost + rotation_cost + current_node->connected[i]->cost;
                // printf("Pos: %d,%d\ntemp_radians: %f\n%s: %f\n\n\n", counter_node->x, counter_node->y, temp_radians, counter_node->name, rot_radians);
                if (temp_cost < counter_node->path_cost) {
                    // printf("Updated: %s, %f, %f\n", counter_node->name, temp_cost, rotation_cost);
                    counter_node->path_cost = temp_cost;
                    // Save the angle we came into the node at
                    counter_node->enter_radians = rot_radians;
                    // We found a better way to get to counter_node, so we update its prev_node reference
                    counter_node->prev_node = current_node;

                    // Make sure that the node is in the array
                    UpdateNodeInArray(node_costs, &node_costs_len, counter_node);
                }
            }
        }
        // printf("cur_node: %s, enter_deg: %f\n", current_node->name, current_node->enter_radians * 180 / PI);
        current_node = GetLowestUndone(node_costs, node_costs_len);
        loop_limiter++;
        if (loop_limiter >= MAX_ITERATIONS) {
            printf("Too many iterations!\n");
            break;
        }
    }
    // printf("cur_node: %s, enter_deg: %f, total_cost: %f\n", current_node->name, current_node->enter_radians * 180 / PI, current_node->path_cost);
    // Dijkstra's is done!
    // Now we can reverse iterate and use the prev_node pointers to find the path the bot should take
    counter_node = target_node;
    final_path->total_cost = target_node->path_cost;
    final_path->path[final_path->top] = counter_node;
    final_path->top++;
    do {
        // Iterate backwards
        counter_node = counter_node->prev_node;

        final_path->path[final_path->top] = counter_node;
        final_path->top++;
    }
    while (counter_node != source_node);
    // printf("%d", final_path->top);
    return final_path;
}

void CurveTowards(Node *source_node, Node *target_node) {
    // These values come from calculating angular velocities and whatnot
    unsigned char fast_value = 255;
    unsigned char slow_value = 210;
    unsigned char left_motor, right_motor;
    float angle = atan2(target_node->y - source_node->y, target_node->x - source_node->x);
    // The angle between the curve nodes are roughly 45 degrees
    // But what matters here is whether it's positive or negative
    // If it's positive, we curve to the left, so the right motor is faster
    if (angle > 0) {
        right_motor = fast_value;
        left_motor = slow_value;
    }
    else {
        right_motor = slow_value;
        left_motor = fast_value;
    }
    // Start the motors on the path for the curve
    pos_encoder_velocity(left_motor, right_motor);
    // Let them keep going until one of the motors has spun enough
    pos_encoder_angle_rotate(angle * 180 / PI);
}

void MoveBotToNode(Node *target_node) {
    PathStack *final_path;
    Node *current_node, *next_node;
    int i;
    float xDist, yDist;

    final_path = Dijkstra(GetCurrentNode(), target_node);
    for (i = final_path->top - 1; i >= 0; i--) {
        printf("%s, ", final_path->path[i]->name);
    }
    printf("\nTotal cost: %f\n", final_path->total_cost);

    // Now that we know the path to take, here's how we actually get there
    // To go from A to D
    // cur_pos = A
    // next_dest = B
    // If curve, use custom function, else
    // Rotate towards next_dest (skip if rotation diff < threshold like 5 degs)
    // Move forward dist using pos encoders
    // Repeat for next pair of nodes
    i = final_path->top - 1;
    current_node = bot_position->cur_node;
    while (current_node != target_node) {
        i--;
        next_node = final_path->path[i];
        // If both the current and next nodes are part of our "curve_nodes", use
        // our curve function
        if (
            IndexOfNode(curve_nodes, curve_nodes_len, current_node) != -1 &&
            IndexOfNode(curve_nodes, curve_nodes_len, next_node) != -1
        ) {
            CurveTowards(current_node, next_node);
        }
        else {
            xDist = current_node->x - next_node->x;
            yDist = current_node->y - next_node->y;
            pos_encoder_rotate_bot((bot_position->cur_radians - next_node->enter_radians) * 180 / PI);
            pos_encoder_forward_mm(sqrt(xDist * xDist + yDist * yDist));
        }

        current_node = next_node;
    }
}