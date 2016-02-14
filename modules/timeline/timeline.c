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


Pizza *CreatePizza(
        char colour,
        char size,
        int order_time,
        char order_type,
        char *delivery_house_name,
        char status
    ) {
    Pizza *new_pizza;
    new_pizza = malloc(sizeof(Pizza));
    new_pizza->colour = colour;
    new_pizza->size = size;
    new_pizza->order_time = order_time;
    new_pizza->order_type = order_type;
    new_pizza->delivery_house = GetNodeByName(delivery_house_name);
    new_pizza->status = 'n';
    new_pizza->pickup_point = NULL;
    if(order_type = 'p'){
        new_pizza->block->start_time = order_time - 30;
        new_pizza->block->end_time = order_time + 30;
    }
    else
    {
        new_pizza->block->start_time = order_time;
        new_pizza->block->end_time = order_time + 30;
    }

    return new_pizza;
}

int check_overlap(Time_Block *a, Time_Block *b){
    if(a->start < b->start && a->end > b->start){
        return 1;
    }

    else if(a->start == b->start && a->end == b->end){
        return 1;
    }

    else if(a->start > b->start && a->start < b->end){
        return 1;
    }

    else 
        return 0;
}

void init_timeline(){
    Pizza **Orders;
    int o, i;
    char colour;
    char size;
    int order_time;
    char order_type;
    char *delivery_house_name;
    printf("No. of pizzas: ");
    scanf("%d", &o);
    Orders = malloc(o * sizeof(Pizza));
    for(i=0; i<o; i++){
        printf("pizza: %d", i);
        scanf("colour: %c\n", &colour);
        scanf("size: %c\n", &size);
        scanf("order time: %d\n", &order_time);
        scanf("order type: %c\n", &order_type);
        scanf("house: %s\n", delivery_house_name);
        Orders[i] = CreatePizza(colour, size, order_time, order_type, delivery_house_name);
    }
}

void block_time(Pizza **Orders){
    int i, j, k = 0;
    int overlap;
    Blocked_Time **block;
    block = malloc(10 * sizeof(Blocked_Time));
    //start time when we pick up pizza from second block
    //end time when we deliver pizza from any one of blocks
    //end time will be changed dynamically as soon a s we deliver the pizza
    for(i=0; i<10; i++){
        for(j=0; j<10; j++){
            overlap = check_overlap(Orders[i]->block, Orders[j]->block);
            if(overlap == 1){
                if(Orders[i]->block->start_time > Orders[j]->block->start_time ){
                    block[k]->start_time = Orders[i]->block->start_time 
                    block[k]->end_time = INFINITY; 
                    k++;
                }
                else {
                    block[k]->start_time = Orders[i]->block->start_time;
                    block[k]->end_time = INFINITY;
                    k++;
                }//end of nested else
            }//end of if statement
        }//end of nester for loop
    }//end of for loop  
}//end of function

void display(Pizza *current_pizza) {
    printf("colour: %c\n", current_pizza->colour);
    printf("size: %c\n", current_pizza->size);
    printf("order time: %d\n", current_pizza->order_time);
    printf("order type: %c\n", current_pizza->order_type);
    printf("house: %s\n", current_pizza->delivery_house->name);
    // printf("pickup point: %s\n", current_pizza->pickup_point);
}
