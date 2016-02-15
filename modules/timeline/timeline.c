#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"
#include "../map/map.h"
#include <math.h>

#define TRUE 1
#define FALSE 0

// Here's how we plan on solving the order timeline
// First, you obviously have to enter the order timeline
// If the latest order is a regular order, we'll try to deliver it ASAP
// If our prediction function says we'll be late for the delivery, we'll consider dropping this delivery
// and moving on to the next one
// If there's time left to the next delivery (a.k.a. free time), we'll consider doing one of 3 things:
// 1) Find more Pizzas
// 2) Pick up preorder Pizzas
// 3) Pick up previously canceled / delayed orders 
// After every delivery is done, we want to consider using it as free time. If we can squeeze some
// extra work in, we might as well
// However, this could backfire in the sense that you *could* get away with taking some free time for the
// next 2 orders, but over time, it'll cost you because you'll be delayed for all orders after that
// Somewhere in the middle of all of this, we also need to consider instances when we will *definitely*
// need 2 arms, like when 1 house orders 2 Pizzas. In this situation, we don't want our second arm to be
// filled already.
// So the best thing to do might be to look for these *definite* needs first and "blocking off" that period
// of time. These blocked off periods will be considered in our "free time" considerations, and any overlapping
// permutations will be dismissed.

Timeline *our_timeline;

// We only have 10 pizza pick up points, and since the rules state that no pizzas will be added
// or removed *during* a run, we can assume that the max orders we'll get is 10
const int MAX_ORDERS = 10;

void CreateOrder(
        Timeline *timeline,
        char colour,
        char size,
        int order_time,
        char order_type,
        char *delivery_house_name
    ) {

    Order *new_order;

    new_order = malloc(sizeof(Order));
    new_order->colour = colour;
    new_order->size = size;
    new_order->order_time = order_time;
    new_order->order_type = order_type;
    new_order->delivery_house = GetNodeByName(delivery_house_name);
    new_order->status = 'n';
    new_order->pickup_point = NULL;
    new_order->block = malloc(sizeof(TimeBlock));

    if (order_type = 'p') {
        new_order->block->start = order_time - 30;
        new_order->block->end = order_time + 30;
    }
    else {
        new_order->block->start = order_time;
        new_order->block->end = order_time + 30;
    }

    timeline->orders[timeline->len] = new_order;
    timeline->len++;
}

void InitTimeline() {
    our_timeline = malloc(sizeof(Timeline));
    our_timeline->orders = malloc(MAX_ORDERS * sizeof(Order));

    CreateOrder(our_timeline, 'r', 'l', 30, 'r', "H12");
    CreateOrder(our_timeline, 'g', 'l', 50, 'p', "H2");
    CreateOrder(our_timeline, 'r', 'l', 60, 'r', "H4");
    CreateOrder(our_timeline, 'b', 'l', 60, 'r', "H4");
    CreateOrder(our_timeline, 'b', 'l', 100, 'r', "H6");
    CreateOrder(our_timeline, 'r', 'l', 130, 'p', "H9");
}

Timeline *GetTimeline() {
    return our_timeline;
}

int CheckOverlap(TimeBlock *a, TimeBlock *b){
    // An overlap occurs if A starts before B ends *and* B starts before A ends.
    if (
        a->start < b->end &&
        b->start < a->end
    ) {
        return TRUE;
    }
    return FALSE;
}

// Find the time block where we'll definitely need 2 arms
// Note that this function *does not* set the end time of the block time
// Since our algorithm doesn't look ahead into the future for delivery permutations
// we leave it to the arm lower function to set the end time
void FindNextDefiniteNeed(Order **Orders){
    int i, j, k = 0;
    int overlap;
    TimeBlock **block;
    block = malloc(10 * sizeof(TimeBlock));
    // The start time is set to when we pick up order from second block,
    // since that's when we start using 2 arms
    // The end time will be changed dynamically as soon as we deliver the order,
    // since that's when we stop using 2 arms
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            overlap = CheckOverlap(Orders[i]->block, Orders[j]->block);
            if (overlap == TRUE) {
                if (Orders[i]->block->start > Orders[j]->block->start) {
                    block[k]->start = Orders[i]->block->start;
                    block[k]->end = INFINITY; 
                    k++;
                }
                else {
                    block[k]->start = Orders[i]->block->start;
                    block[k]->end = INFINITY;
                    k++;
                }
            }
        }
    }
}

void display(Order *current_order) {
    printf("colour: %c\n", current_order->colour);
    printf("size: %c\n", current_order->size);
    printf("order time: %d\n", current_order->order_time);
    printf("order type: %c\n", current_order->order_type);
    printf("house: %s\n", current_order->delivery_house->name);
    // printf("pickup point: %s\n", current_order->pickup_point);
}
