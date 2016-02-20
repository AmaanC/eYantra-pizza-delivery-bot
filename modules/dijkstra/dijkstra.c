#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dijkstra.h"
#include "../map/map.h"

#define TRUE 1
#define FALSE 0

// The things you need to do to initialize all the nodes for Dijkstra's algorithm
// Namely, set the path_cost to infinite
// And set the done flag to FALSE
int InitNodesDijkstra(Node *current_node) {
    current_node->path_cost = INFINITY; // INFINITY is a macro from math.h
    current_node->done = FALSE;
    return FALSE;
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

// Use Dijkstra's algorithm to figure out best path and then use
// move_to module to move the bot through all the nodes
// in the route we choose. (move_to uses the pos_encoder and bl_sensor to
// ensure that the bot travels exactly to the point we want it to.)
// For eg. the path fro going from A -> C is A->B->C
// This function will figure that path out and call move_to(B.x, B.y)
// first. Then it'll call move_to(C.x, C.y) when it is done.
// It'll also update the current_node variable, of course.
PathStack* Dijkstra(Node *source_node, Node *target_node, float cur_deg, Graph *our_graph) {
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
    // rot_deg is the new enter_deg for node B
    // temp_deg is the angle the bot will start from at node A
    float accum_cost = 0;
    float temp_cost = 0;
    float rotation_cost = 0;
    float rot_deg = 0;
    float temp_deg = 0;
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
    // Curve info
    CurveInfo *curve_info;

    // The final_path won't always be this long, but we need enough memory in case it is, somehow
    final_path = malloc(sizeof(PathStack));
    final_path->top = 0;
    if (source_node == target_node) {
        return final_path;
    }
    final_path->path = malloc(our_graph->num_nodes * sizeof(Node*));

    curve_info = GetCurveInfo();

    current_node = source_node;
    DFSEval(source_node, source_node->visited, InitNodesDijkstra);
    node_costs = malloc(our_graph->num_nodes * sizeof(Node*));

    source_node->path_cost = 0;
    source_node->done = TRUE;
    source_node->enter_deg = cur_deg;
    loop_limiter = 0;

    // lcd_printf("Targ: %s", target_node->name);
    // _delay_ms(200);
    if (current_node == NULL || target_node == NULL) {
        printf("ERROR: NULL pointer for source or target - dijkstra.c\n\n\n");
        final_path->total_cost = INFINITY;
        return final_path;
    }
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
                temp_deg = current_node->enter_deg;
                // the angle between the current node & the counter node
                // atan2 always returns in the range of -pi to pi, so we convert it to degrees
                rot_deg = RadToDeg(atan2(counter_node->y - current_node->y, counter_node->x - current_node->x));
                rotation_cost = GetRotationCost(temp_deg - rot_deg);
                // If we're considering the cost of going through a curve (i.e. we're at A and considering B
                // as the counter_node), we should not be considering any rot_cost, since no rotation is 
                // required before movement
                if (
                    IndexOfNode(curve_info->curve_nodes, curve_info->curve_nodes_len, current_node) != -1 &&
                    IndexOfNode(curve_info->curve_nodes, curve_info->curve_nodes_len, counter_node) != -1
                ) {
                    rotation_cost = 0;
                    // rot_deg is used to set enter_deg for the counter_node, which will be +90 deg of the current
                    // one if we move anticlockwise, i.e. with the right motor faster
                    rot_deg = current_node->enter_deg + 90 * GetCurveDirection(current_node, counter_node);
                }

                temp_cost = accum_cost + rotation_cost + current_node->connected[i]->cost;
                // printf("Pos: %f,%f\ntemp_deg: %f\n%s: %f\n\n\n", counter_node->x, counter_node->y, temp_deg, counter_node->name, MakePositiveDeg(rot_deg));
                if (temp_cost < counter_node->path_cost) {
                    // printf("Updated: %s, %f, %f\n", counter_node->name, temp_cost, rotation_cost);
                    counter_node->path_cost = temp_cost;
                    // Save the angle we came into the node at
                    counter_node->enter_deg = MakePositiveDeg(rot_deg);
                    // printf("Enter: %f\n", counter_node->enter_deg);
                    // We found a better way to get to counter_node, so we update its prev_node reference
                    counter_node->prev_node = current_node;

                    // Make sure that the node is in the array
                    UpdateNodeInArray(node_costs, &node_costs_len, counter_node);
                }
            }
        }
        // printf("cur_node: %s, enter_deg: %f\n", current_node->name, current_node->enter_deg * 180 / M_PI);
        current_node = GetLowestUndone(node_costs, node_costs_len);
        loop_limiter++;
        if (loop_limiter >= MAX_ITERATIONS) {
            printf("Too many iterations!\n");
            break;
        }
    }
    // printf("cur_node: %s, enter_deg: %f, total_cost: %f\n", current_node->name, current_node->enter_deg, current_node->path_cost);
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