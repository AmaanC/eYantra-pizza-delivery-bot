#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timeline.h"
#include "../map/map.h"
#include "../dijkstra/dijkstra.h"
#include "../bot_memory/bot_memory.h"
#include "../timer-gcc/timer.h"
#include "../hardware_control/hardware_control.h"

#define TRUE 1
#define FALSE 0

// real_pizza: whether we're checking if there's a real pizza there or if we're checking if
// a location has been allocated
char IsPizzaAt(Node *test_node, char real_pizza) {
    unsigned char i = 0;
    char found = FALSE;
    Pizza *current_pizza;
    PizzaList *our_pizzas;
    our_pizzas = GetPizzas();
    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        if (
            (current_pizza->location == test_node || current_pizza->dep_loc == test_node) &&
            current_pizza->found == real_pizza
        ) {
            found = TRUE;
            // printf("  Pizza at %s", test_node->name);
        }
    }
    return found;
}

// Returns a node which is to the right (where x co-ordinate is higher) of the source_node
// It only looks through the source_nodes connections, not through all the nodes on the map
Node *GetNodeToRight(Node *source_node) {
    unsigned char i;
    Node *counter_node;
    for (i = 0; i < source_node->counter; i++) {
        counter_node = source_node->connected[i]->ptr;
        if (counter_node->x > source_node->x) {
            return counter_node;
        }
    }
    return NULL;
}

Node *GetNodeToLeft(Node *source_node) {
    unsigned char i;
    Node *counter_node;
    for (i = 0; i < source_node->counter; i++) {
        counter_node = source_node->connected[i]->ptr;
        if (counter_node->x < source_node->x) {
            return counter_node;
        }
    }
    return NULL;
}

// Gets the first vacant pizza counter node to the right
Node *GetFirstPToRight(char real_pizza) {
    Node *right;
    right = GetNodeToRight(GetPizzaCounter());
    while (right != NULL && IsPizzaAt(right, real_pizza) == TRUE) {
        right = GetNodeToRight(right);
    }
    return right;
}

// Gets the first vacant pizza counter node to the left
Node *GetFirstPToLeft(char real_pizza) {
    Node *left;
    left = GetNodeToLeft(GetPizzaCounter());
    while (left != NULL && IsPizzaAt(left, real_pizza) == TRUE) {
        left = GetNodeToLeft(left);
    }
    return left;
}

// Get the furthest unknown pizza counter from the source_node
Node *GetFurthestPizzaNode(Node *source_node) {
    Node *left_counter, *right_counter, *furthest_node;
    Graph *our_graph = GetGraph();
    float cost = INFINITY;
    float max_cost = 0;
    furthest_node = NULL;
    
    left_counter = right_counter = GetPizzaCounter();

    left_counter = GetNodeToLeft(left_counter);
    right_counter = GetNodeToRight(right_counter);
    while (left_counter != NULL && right_counter != NULL) {
        cost = Dijkstra(source_node, left_counter, source_node->enter_deg, our_graph)->total_cost;
        if (cost > max_cost) {
            max_cost = cost;
            furthest_node = left_counter;
        }
        cost = Dijkstra(source_node, right_counter, source_node->enter_deg, our_graph)->total_cost;
        if (cost > max_cost) {
            max_cost = cost;
            furthest_node = right_counter;
        }

        left_counter = GetNodeToLeft(left_counter);
        right_counter = GetNodeToRight(right_counter);
    }
    return furthest_node;
}

// TODO: Consider adding a threshold. An overlap of 1s means very little
char CheckOverlap(TimeBlock *a, TimeBlock *b){
    // An overlap occurs if A starts before B ends *and* B starts before A ends.
    if (
        a->start < b->end &&
        b->start < a->end
    ) {
        return TRUE;
    }
    return FALSE;
}

TimeBlock *GetCurrentTimeBlock() {
    TimeBlock *current;
    current = malloc(sizeof(TimeBlock));
    // TODO: USE TIMER TO GET CURRENT TIME
    // current->start = TimerGetTime();
    current->start = GetCurrentTime();
    current->end = current->start + 5;
    return current;
}
