// Trying to follow Google's style guide (for C++, since they don't have one for C)
// Source: https://google.github.io/styleguide/cppguide.html
// Important to keep in mind: https://google.github.io/styleguide/cppguide.html#General_Naming_Rules

#define TRUE 1
#define FALSE 0
// #define M_PI 3.14159265

// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "map.h"
#include "../dijkstra/dijkstra.h"
#include "../pos_encoder/pos_encoder.h"
#include "../lcd/lcd.h"
#include "../bot_memory/bot_memory.h"

Graph *our_graph;

Node *found_node; // The node we find by name in our timeline solver
char *current_search_name; // The name of the node we're looking for in the timeline solver

CurveInfo *curve_info;

BotInfo *bot_info;

// Returns a pointer to a node
Node *CreateNode(float x, float y, int num_connected, char *name) {
    int i;
    Node *new_node;
    new_node = malloc(sizeof(Node));
    new_node->name = malloc(5); // 5 char array for the name

    new_node->x = x;
    new_node->y = y;
    new_node->num_connected = num_connected;
    new_node->name = name;
    new_node->counter = 0;
    new_node->visited = 0;
    new_node->done = 0;

    // new_node->prev_node = malloc(sizeof(Node));
    new_node->connected = malloc(new_node->num_connected * sizeof(Connection*)); // Create space for an array of connection pointers
    for (i = 0; i < num_connected; i++) {
        new_node->connected[i] = malloc(sizeof(Connection)); // Create space for every individual connection in the array
    }

    our_graph->num_nodes++;

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

    bot_info = GetBotInfo();
    
    curve_info = malloc(sizeof(CurveInfo));
    curve_info->curve_nodes_len = 8;
    curve_info->curve_nodes = malloc(curve_info->curve_nodes_len * sizeof(Node*));

    our_graph = malloc(sizeof(Graph));
    our_graph->num_nodes = 0;

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
    our_graph->start = S;
    bot_info->cur_position->cur_node = S;
    bot_info->cur_position->cur_deg = 90;

    // Define the nodes where we want to use our custom curve function instead of
    // rotating towards the next node and going forward
    // Which direction we're curving in is done by using atan2 and finding the 
    // angle between the nodes
    curve_info->curve_nodes[0] = r3;
    curve_info->curve_nodes[1] = r4;
    curve_info->curve_nodes[2] = r6;
    curve_info->curve_nodes[3] = r7;
    curve_info->curve_nodes[4] = r9;
    curve_info->curve_nodes[5] = r10;
    curve_info->curve_nodes[6] = r12;
    curve_info->curve_nodes[7] = r13;

    curve_info->curve_center = r18;

    // lcd_printf("Added");
    // _delay_ms(100);
}

Node *GetCurrentNode() {
    return bot_info->cur_position->cur_node;
}

// Call it like this:
// DFSEval(GetCurrentNode(), GetCurrentNode()->visited, update_dist)
void DFSEval(Node *source_node, int unvisited_value, int fn()) {
    int i;
    // printf("DFS: %s\n", source_node->name);
    source_node->visited = !unvisited_value;

    fn(source_node);
    for (i = 0; i < source_node->counter; i++) {
        // If it hasn't been visited already, run DFS on the node too.
        if (source_node->connected[i]->ptr->visited == unvisited_value) {
            DFSEval(source_node->connected[i]->ptr, unvisited_value, fn);
        }
    }
}


// This uses 2 global variables, current_search_name and found_node
// I realize that this is terrible, but I picked this method over
// having N different functions, all of which used DFS for different logic
// So you've been warned, this function has side effects
int CheckNodeName(Node *current_node) {
    // printf("Checking %s vs. %s\n", current_node->name, current_search_name);
    if (strcmp(current_node->name, current_search_name) == 0) {
        found_node = current_node;
        return TRUE;
    }
    return FALSE;
}

Node *GetNodeByName(char *name) {
    Node *start_node;
    start_node = our_graph->start;
    found_node = NULL;
    current_search_name = name;
    // CheckNodeName automatically modifies a global variable called found_node, so we can just return that
    DFSEval(start_node, start_node->visited, CheckNodeName);
    if (found_node == NULL) {
        printf("ERROR: found_node for %s is NULL\n\n\n", name);
        return our_graph->start;
    }
    return found_node;
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

CurveInfo *GetCurveInfo() {
    return curve_info;
}

Graph *GetGraph() {
    return our_graph;
}

// Time taken for the bot to turn X deg. Needs to factor into Dijkstra's
float GetRotationCost(float deg) {
    // TODO: Use actual measured value
    return 0.1 * fabs(deg);
}

float RadToDeg(float radians) {
    return radians * 180 / M_PI;
}

float MakePositiveDeg(float angle) {
    if (angle < 0) {
        return MakePositiveDeg(angle + 360);
    }
    else {
        return fmod(angle, 360);
    }
}

float MakePositiveRad(float angle) {
    if (angle < 0) {
        return MakePositiveDeg(angle + 2 * M_PI);
    }
    else {
        return fmod(angle, 2 * M_PI);
    }
}

// Get the angular velocity to travel from node1 to node2 on a curve in 1 second
float GetAngularVelocity(Node *node1, Node *node2) {
    float angular_velocity = 0;

    // To find the angle formed if you joined the center to curve_node_1 and curve_node_2
    float angle_to_1 = atan2(curve_info->curve_center->y - node1->y, curve_info->curve_center->x - node1->x);
    float angle_to_2 = atan2(curve_info->curve_center->y - node2->y, curve_info->curve_center->x - node2->x);

    // Bring it in the range of 0-2pi so +ves and -ves don't throw us off
    // In 1 second, we need to turn X radians, as calculated here
    angular_velocity = MakePositiveRad(angle_to_2) - MakePositiveRad(angle_to_1);

    return angular_velocity;
}

// Returns 1 if the right motor should be the faster one
// -1 if the left should be faster
int GetCurveDirection(Node *source_node, Node *target_node) {
    float angular_velocity = GetAngularVelocity(source_node, target_node);
    // We calculate the angular velocity by seeing the angle between the target_node from the center
    // and the source_node from the center. If this angle is positive, we're moving in the anticlockwise
    // direction
    // printf("%f, %f\n", source_node->enter_deg, angular_velocity);
    if (angular_velocity >= 0) {
        return 1;
    }
    return -1;
}

void CurveTowards(Node *source_node, Node *target_node) {
    // We can use the center of the circle and a curve node to find the radius
    // Using the radius, we can calculate the angular velocities required by
    // each of the wheels
    unsigned char fast_value = 0;
    unsigned char slow_value = 0;
    unsigned char left_motor, right_motor;
    unsigned char max_speed = 0xFF; // The max speed either motor can turn at
    const wheel_dist = 7.5; // According to measurement
    float ratio = 1;
    float angular_velocity = 0;

    float radius = 0; // The radius will be calculated dynamically
    // Needed to find the radius
    float xDist = source_node->x - curve_info->curve_center->x;
    float yDist = source_node->y - curve_info->curve_center->y;
    radius = sqrt(xDist * xDist + yDist * yDist);
    printf("%f, %f\n", source_node->x, yDist);

    angular_velocity = GetAngularVelocity(source_node, target_node);
    // Linear velocity = radius * angular velocity
    // Our motors have different radiuses, which is why they have different linear velocities
    slow_value = (radius - wheel_dist) * fabs(angular_velocity);
    fast_value = (radius + wheel_dist) * fabs(angular_velocity);
    ratio = max_speed / fast_value;

    fast_value = max_speed;
    slow_value *= ratio;
    
    // if (GetCurveDirection(source_node, target_node) == 1) {
    if (angular_velocity > 0) {
        right_motor = fast_value;
        left_motor = slow_value;
    }
    else {
        right_motor = slow_value;
        left_motor = fast_value;
    }
    printf("\tCurve %s to %s: %d, %d\n", source_node->name, target_node->name, left_motor, right_motor);
    // Start the motors on the path for the curve
    // pos_encoder_velocity(left_motor, right_motor);
    // Let them keep going until one of the motors has spun enough
    // pos_encoder_forward();
    // pos_encoder_angle_rotate(abs(angle * 180 / M_PI));
}

void MoveBotToNode(Node *target_node) {
    PathStack *final_path;
    Node *current_node, *next_node;
    int i;
    float xDist, yDist;

    final_path = Dijkstra(GetCurrentNode(), target_node, bot_info->cur_position->cur_deg, our_graph);
    for (i = final_path->top - 1; i >= 0; i--) {
        // lcd_printf("%s", final_path->path[i]->name);
        // _delay_ms(500);
        printf("%s, ", final_path->path[i]->name);
    }
    // lcd_printf("Cost: %d", (int) final_path->total_cost);
    // _delay_ms(500);

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
    current_node = bot_info->cur_position->cur_node;
    while (current_node != target_node) {
        i--;
        next_node = final_path->path[i];
        // If both the current and next nodes are part of our "curve_nodes", use
        // our curve function
        if (
            IndexOfNode(curve_info->curve_nodes, curve_info->curve_nodes_len, current_node) != -1 &&
            IndexOfNode(curve_info->curve_nodes, curve_info->curve_nodes_len, next_node) != -1
        ) {
            // lcd_printf("Curve");

            CurveTowards(current_node, next_node);
        }
        else {

            xDist = current_node->x - next_node->x;
            yDist = current_node->y - next_node->y;
            // lcd_printf("Rot: %d", (int) ((next_node->enter_deg - bot_info->cur_position->cur_deg)));
            // _delay_ms(500);
            // pos_encoder_rotate_bot((next_node->enter_deg - bot_info->cur_position->cur_deg));
            bot_info->cur_position->cur_deg = next_node->enter_deg;
            // pos_encoder_forward_mm(10 * sqrt(xDist * xDist + yDist * yDist));
        }

        bot_info->cur_position->cur_node = current_node;
        current_node = next_node;
    }
    bot_info->cur_position->cur_node = target_node;
}