#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"

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


Pizza* insert_values(
		char colour,
		char size,
		int order_time,
		char order_type,
		char *delivery_house,
		char *pickup_point
	) {
	Pizza* temp;
	temp = malloc(sizeof(Pizza));
	temp->colour = colour;
	temp->size = size;
	temp->order_time = order_time;
	temp->order_type = order_type;
	temp->delivery_house = delivery_house;
	temp->pickup_point = pickup_point;
	return temp;
}

void display(Pizza* temp)
{
	printf("colour: %c\n", temp->colour);
	printf("size: %c\n", temp->size);
	printf("order time: %d\n", temp->order_time);
	printf("order type: %c\n", temp->order_type);
	printf("house: %s\n", temp->delivery_house);
	printf("pickup point: %s\n", temp->pickup_point );
}
