#include <unistd.h>
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

// We maintain a list of orders
OrderList *our_timeline;

// and a list of pizzas
PizzaList *our_pizzas;

// Number of pizzas found so we know when to stop
int total_pizzas = 0;

int total_orders = 0;
int orders_completed = 0;

// We maintain the two lists independently because, for example,
// H2 and H8 can both have ordered small green pizzas.
// We need to separate the pizza's identity (and personality) from their orders
// It doesn't matter *which* small green pizza goes where.
// All that matters is that both houses get a tasty small green pizza!
// So when we want to deliver a pizza, we'll see if that pizza has been found
// and if it has, only then do we consider delivering it.
// Else we'll consider searching
// For when we're considering using 2 arms, we'll make sure that 2 different orders
// aren't both considering the same pizza, so we'll need a counter of sorts
// or some kind of state

Node *PIZZA_COUNTER_NODE;

// State can be either 'f' or 'b' for free and busy
// When it's in the free state, it'll consider doing extra things in its free time like
// looking for more pizzas
// In its busy state, it'll find the next pizza due on the list, and just deliver that
char cur_state = 'f';

// A time period when we will *require* 2 arms
// Since it's end period is set dynamically, every time that we stop requiring it
// we can check the future orders again for another "definite need"
TimeBlock *next_required_period;

// This may be NULL often. It'll be the pizza we pick up using our extra arm when our FreeTime function
// thinks it's a good idea.
// To clarify, FreeTime will only set this variable to an order.
// The NormalOperation function is the one that will actually pick it up
// and deliver it.
Order *next_extra_order;

// We only have 10 pizza pick up points, and since the rules state that no pizzas will be added
// or removed *during* a run, we can assume that the max orders we'll get is 10
const int MAX_ORDERS = 10;
const int GUARANTEED_PERIOD = 50;

char GetState() {
    return cur_state;
}

void SetState(char new_state) {
    if (new_state == 'f') {
        cur_state = new_state;
    }
    else {
        cur_state = 'b';
    }
}

Node *GetPizzaCounter() {
    return PIZZA_COUNTER_NODE;
}

void InsertOrder(OrderList *timeline, Order *new_order) {
    int i = 0;
    i = timeline->len;
    // Here, we want to insert the new_order into the array and keep the
    // array sorted by their delivery period's end time in ascending order

    // Reason for the decision:
    // For example, if there's a reg order at time 50, and a preorder at time 70
    // We can pick up the preorder at 40 and the reg at 50
    // We *have* to deliver the pre by 100 and the reg by 80
    // By default, we want to maximize deliveries in time, and to do that
    // I'll pick up the pizza which is due right now over the one
    // that is due later.
    // Therefore, sort by due time.

    // To do so, we shift the array's elements to the right until we find
    // the new_order's spot. Then we insert the new_order at that spot
    while (
        i > 0 &&
        timeline->orders[i - 1] != NULL &&
        new_order->delivery_period->end < timeline->orders[i - 1]->delivery_period->end
    ) {
        timeline->orders[i] = timeline->orders[i - 1];
        i--;
    }
    timeline->orders[i] = new_order;
    timeline->len++;
}

void InsertPizza(PizzaList *pizza_list, Pizza *new_pizza) {
    pizza_list->pizzas[pizza_list->len] = new_pizza;
    pizza_list->len++;
}

Pizza *CreatePizza(char colour, char size) {
    Pizza *pizza;

    pizza = malloc(sizeof(Pizza));
    pizza->colour = colour;
    pizza->size = size;
    pizza->found = FALSE;
    pizza->state = 'f';
    pizza->location = GetFirstPToRight(FALSE);
    if (pizza->location == NULL) {
        pizza->location = GetFirstPToLeft(FALSE);
    }

    // Insert into our array of pizzas
    InsertPizza(our_pizzas, pizza);

    return pizza;
}

void CreateOrder(
        OrderList *timeline,
        char colour,
        char size,
        int order_time,
        char order_type,
        char *delivery_house_name
    ) {

    Order *new_order;
    int i = 0;

    CreatePizza(colour, size);
    new_order = malloc(sizeof(Order));

    new_order->colour = colour;
    new_order->size = size;
    new_order->order_time = order_time;
    new_order->order_type = order_type;
    new_order->delivery_house = GetNodeByName(delivery_house_name);
    new_order->state = 'n';
    new_order->delivery_period = malloc(sizeof(TimeBlock));

    if (order_type == 'p') {
        new_order->pickup_time = 0;
        new_order->delivery_period->start = order_time - GUARANTEED_PERIOD;
        new_order->delivery_period->end = order_time + GUARANTEED_PERIOD;
    }
    else {
        new_order->pickup_time = order_time;
        new_order->delivery_period->start = order_time;
        new_order->delivery_period->end = order_time + GUARANTEED_PERIOD;
    }

    total_orders++;
    InsertOrder(timeline, new_order);
// //////    // printf("%d: %s\n", i, delivery_house_name);
}

void InitTimeline() {
    our_timeline = malloc(sizeof(OrderList));
    our_timeline->orders = malloc(MAX_ORDERS * sizeof(Order));
    our_timeline->len = 0;

    our_pizzas = malloc(sizeof(PizzaList));
    our_pizzas->pizzas = malloc(MAX_ORDERS * sizeof(Pizza));

    PIZZA_COUNTER_NODE = GetNodeByName("r1"); // r1 is always going to be our "pizza counter"

    next_required_period = malloc(sizeof(TimeBlock));
    next_required_period->start = next_required_period->end = 0;

    // For reference:
    // CreateOrder(OrderList *timeline, char colour, char size, int order_time, char order_type, char *delivery_house_name)
    // CreateOrder(our_timeline, 'r', 'l', 30, 'r', "H12");
    // CreateOrder(our_timeline, 'g', 'l', 70, 'p', "H2");
    // CreateOrder(our_timeline, 'r', 's', 50, 'r', "H4");
    // CreateOrder(our_timeline, 'b', 'm', 89, 'r', "H4");
    // CreateOrder(our_timeline, 'b', 'l', 0, 'r', "H6");
    // CreateOrder(our_timeline, 'r', 's', 30, 'p', "H6");

    CreateOrder(our_timeline, 'b', 'l', 30, 'r', "H6");
    CreateOrder(our_timeline, 'r', 's', 30, 'p', "H6");
    CreateOrder(our_timeline, 'r', 'l', 70, 'r', "H12");
    CreateOrder(our_timeline, 'r', 's', 110, 'r', "H4");
    CreateOrder(our_timeline, 'g', 'l', 150, 'p', "H2");
    CreateOrder(our_timeline, 'b', 'm', 220, 'r', "H4");
}

OrderList *GetTimeline() {
    return our_timeline;
}

PizzaList *GetPizzas() {
    return our_pizzas;
}

// Find the time block where we'll definitely need 2 arms
// Note that this function *does not* set the end time of the block time
// Since our algorithm doesn't look ahead into the future for delivery permutations
// we leave it to the arm lower function to set the end time
void FindNextDefiniteNeed(OrderList *timeline) {
    int i, j;
    Order *order1, *order2;
    float start_time = INFINITY; // The potential start time of the next_required_period
    float lowest_start_time = INFINITY; // The lowest required period we've found so far
    TimeBlock *next_potential; // Potentially the next_required_period

    // If there's a required_period going on right now, there's no point to checking for the next one
    // until this one ends
    if (CheckOverlap(GetCurrentTimeBlock(), next_required_period)) {
        return;
    }

    next_potential = malloc(sizeof(TimeBlock));
    next_potential->start = INFINITY;
    next_potential->end = INFINITY;

    // We'll look at every combination of the orders we've got and see if any of them have an overlapping period
    // and are going to the same house.
    // This is the only situation we see where we *definitely* need to use our 2 arms
    // If the orders go to different houses, we might spend more time and might get delayed for
    // our other orders, so this isn't a *definite* thing, it's something to consider, which
    // will be done by our algorithm in its "free time" state.
    // Note, though, that we can only consider finding our next definite need if our current one
    // is over
    // This function needs to find the *next* required period, not *a* required period.
    // So we need the one with the timeframe being as soon as possible compared to right now.
    for (i = 0; i < timeline->len; i++) {
        for (j = 0; j < timeline->len; j++) {
            if (i == j) {
                continue;
            }
            order1 = timeline->orders[i];
            order2 = timeline->orders[j];

            // The start time is set to when we pick up the order from second block,
            // since that's when we start using 2 arms
            // The end time will be changed dynamically as soon as we deliver the order,
            // since that's when we stop using 2 arms
            if (order1->delivery_period->start > order2->delivery_period->start) {
                start_time = order1->delivery_period->start;
            }
            else {
                start_time = order2->delivery_period->start;
            }

            // If it's the lowest one we've found
            // and going to the same house
            // and their timings overlap
            if (
                start_time < lowest_start_time &&
                order1->delivery_house == order2->delivery_house &&
                CheckOverlap(order1->delivery_period, order2->delivery_period) == TRUE
            ) {
                lowest_start_time = start_time;
                next_potential->start = start_time;
// //////                printf("Overlap between %s, %f and %f\n", order1->delivery_house->name, order1->delivery_period->start, order2->delivery_period->start);
            }
            
        }
    }
// //////    printf("Next period: %f to %f\n", next_potential->start, next_potential->end);
    // Out of all the required periods available, the one with the lowest time
    // is set to next_potential, so we set that to the next_required_period
    next_required_period = next_potential;
}

// Gets the number of orders that will be delayed if we start at source_node
// and start_time for order_num
// This is used to check if canceling an order will be beneficial
// We're trying to see if our future orders will suffer
// For example, 
// If order A is delivered, we'll start from order A's delivery location
// at the time when we'll be able to deliver it
// So source_node = A.delivery_house and start_time = time when we reach the delivery house
// order_num = 1 since we're not considering the current order, we're looking into the future
// The function is recursive, so it keeps going and sees how many future orders will be
// delayed if we start for the next order after delivering the current order

// This is compared to canceling the current order and changing
// source_node = bot's current location and
// start_time = GetCurrentTime() because we're simulating canceling
// our current order, which means we can get to the one after immediately 
int GetNumDelayed(Node *source_node, int start_time, int order_num) {
    int delayed = 0;
    float cost_to_pick = INFINITY;
    float cost_to_deliver = INFINITY;
    float total_cost = INFINITY;
    Order *next_order = GetNextOrder(our_timeline, order_num);
    Pizza *order_pizza = GetPizzaForOrder(next_order);
    Graph *our_graph = GetGraph();
    if (next_order == NULL) {
        // No future orders left, so delayed = 0
        printf("NULL next\n");
        return 0;
    }
    Display(next_order);
    // printf("Num delayed %d %d\n", order_pizza == NULL, source_node == NULL);
    // printf("%d\n", order_pizza->location == NULL);

    // We don't care about using guessed pizza locations, as long as
    // the calls compare in a similar manner.
    cost_to_pick = Dijkstra(source_node, order_pizza->location, source_node->enter_deg, our_graph)->total_cost;
    // If we get there before we can pick it up, our cost to pick is actually higher
    if (start_time + cost_to_pick < next_order->pickup_time) {
        cost_to_pick = next_order->pickup_time - start_time;
    }
    cost_to_deliver = Dijkstra(order_pizza->location, next_order->delivery_house, order_pizza->location->enter_deg, our_graph)->total_cost;
    total_cost = start_time + cost_to_pick + cost_to_deliver;
    // If we reach after the delivery period ends, we've delayed this order
    if (total_cost > next_order->delivery_period->end) {
        delayed = 1;
    }
    // We set the state to "considering" so that when the function is called recursively,
    // the next order doesn't try to access the same pizza
    order_pizza->state = 'c';
    // Total future orders delayed is how many orders 
    delayed += GetNumDelayed(order_pizza->location, total_cost, order_num + 1);
    order_pizza->state = 'f';
    return delayed;
}

// Consider canceling an order in case it'll take too long or it'll delay our next order

// When called from FreeTime, order1 will be the next_reg_order and
// order2 will be the one we're considering picking up in our free time
// This function will return 1 if you should cancel for this combination
// because the combo delays our later orders

// When called from NormalOperation, order1 will be the next_reg_order and
// order2 will be NULL. It'll calculate the cost of delivering order1
// and if it doesn't delay our next orders, we'll return 0.
DeliverySequence *ConsiderCancel(Order *order1, Order *order2) {
    int i = 0;
    int j = 0;
    int a = 0;
    int b = 0;
    const int num_combos = 2;
    // An array of order pointers
    Order *order_combo[] = {order1, order2};
    Order *single_order;
    Pizza *pizza_combo[2];
    Pizza *single_pizza;

    BotInfo *bot_info = GetBotInfo();
    Graph *our_graph = GetGraph();
    DeliverySequence *best_seq;
    best_seq = malloc(sizeof(DeliverySequence));
    best_seq->should_cancel = FALSE;
    best_seq->guess1 = FALSE;
    best_seq->guess2 = FALSE;

    // Number of regular orders delayed if deliver the current order vs. if we cancel it
    int num_delayed_if_deliver = 0;
    int num_delayed_if_cancel = 0;

    printf("ConsiderCancel %d %d\n\n", order1 == NULL, order2 == NULL);

    float lowest_cost = INFINITY;
    float temp_cost = INFINITY;
    float cost_to_pick1 = INFINITY;
    float cost_to_pick2 = INFINITY;
    float cost_to_deliver1 = INFINITY;
    float cost_to_deliver2 = INFINITY;

    // If either order is null, then there's only one way to deliver the remaining order
    // Go pick it up, deliver it
    if (order1 == NULL || order2 == NULL) {
        if (order2 == order1) {
            // printf("Both orders are NULL, doofus.\n");
            return NULL;
        }
        // printf("Single order\n");
        single_order = (order1 == NULL) ? order2 : order1;
        single_pizza = GetPizzaForOrder(single_order);
        // Cost to go pick the pizza up
        cost_to_pick1 = Dijkstra(bot_info->cur_position->cur_node, single_pizza->location, bot_info->cur_position->cur_deg, our_graph)->total_cost;
        // If we get there before we're allowed to pick the pizza up
        // then we want to consider the waiting time as well
        if (GetCurrentTime() + cost_to_pick1 < single_order->pickup_time) {
            cost_to_pick1 = single_order->pickup_time - GetCurrentTime();
        }
        // Plus cost of delivering pizza
        temp_cost = cost_to_pick1 + Dijkstra(single_pizza->location, single_order->delivery_house, single_pizza->location->enter_deg, our_graph)->total_cost;

        best_seq->order1 = single_order;
        best_seq->pick1 = single_pizza->location;
        best_seq->deliver1 = single_order->delivery_house;
        best_seq->total_cost = temp_cost;

        printf("House null: %d\n", single_order->delivery_house == NULL);
        num_delayed_if_deliver = GetNumDelayed(single_order->delivery_house, GetCurrentTime() + temp_cost, 1);
        num_delayed_if_cancel = GetNumDelayed(GetCurrentNode(), GetCurrentTime(), 1);
        // If we delay fewer pizzas later by canceling, then lets cancel
        printf("Delayed: %d %d\n", num_delayed_if_cancel, num_delayed_if_deliver);
        if (num_delayed_if_cancel < num_delayed_if_deliver) {
            best_seq->should_cancel = TRUE;
        }
        // printf("Should cancel: %d\n", best_seq->should_cancel);
        return best_seq;
    }


    printf("Getting pizzas for orders\n");
    Display(order1);
    Display(order2);
    pizza_combo[0] = GetPizzaForOrder(order1);
    pizza_combo[1] = GetPizzaForOrder(order2);

    printf("*** Multiple orders %d, %d, %d, %d\n", pizza_combo[0] == NULL, pizza_combo[1] == NULL, order_combo[0] == NULL, order_combo[1] == NULL);

    // //printf("%d\n", pizza_combo[0]->location == NULL);


    // If, however, there *are* 2 orders, we can pick them up in 2 ways: a, b and b, a
    // And we can deliver them in 2 ways a, b and b, a after having picked both up
    // In total, there are 4 ways (p indicates pick, d indicates deliver)
    // p(a), p(b), d(a), d(b)
    // p(a), p(b), d(b), d(a)
    // p(b), p(a), d(a), d(b)
    // p(b), p(a), d(b), d(a)

    // Here's a for loop ladder
    // It's messy, I know, but it's better than repeating myself for all the different combinations!
    // Plus, this is semi-scalable
    // It's basically creating the following pattern:
    // 0, 1 - 0, 1
    // 0, 1 - 1, 0
    // 1, 0 - 0, 1
    // 1, 0 - 1, 0

    best_seq->should_cancel = TRUE;
    for (i = 0; i < num_combos; i++) {
        for (j = 0; j < num_combos; j++) {
            if (i != j) {
                for (a = 0; a < num_combos; a++) {
                    for (b = 0; b < num_combos; b++) {
                        // //// printf("aaaa%d, %d - %d, %d\n", i, j, a, b);
                        if (
                            a != b &&
                            pizza_combo[i]->location != NULL &&
                            pizza_combo[j]->location != NULL &&
                            order_combo[a]->delivery_house != NULL &&
                            order_combo[b]->delivery_house != NULL
                        ) {
                            // //printf("%d, %d - %d, %d\n", i, j, a, b);
                            // Get cost to pick i
                            // If we get there before we can pick it up, we have to wait until
                            // it's time to pick them up
                            cost_to_pick1 = Dijkstra(bot_info->cur_position->cur_node, pizza_combo[i]->location, bot_info->cur_position->cur_deg, our_graph)->total_cost;
                            if (GetCurrentTime() + cost_to_pick1 < order_combo[i]->pickup_time) {
                                cost_to_pick1 = GetCurrentTime() - order_combo[i]->pickup_time;
                            }
                            temp_cost = cost_to_pick1;

                            // To pick j
                            // If we get there before we can pick it up, we have to wait until
                            // it's time to pick them up
                            cost_to_pick2 = Dijkstra(pizza_combo[i]->location, pizza_combo[j]->location, pizza_combo[i]->location->enter_deg, our_graph)->total_cost;
                            if (cost_to_pick1 + cost_to_pick2 < order_combo[j]->pickup_time) {
                                cost_to_pick2 = GetCurrentTime() - order_combo[j]->pickup_time;
                            }
                            temp_cost += cost_to_pick2;

                            // To deliver a
                            cost_to_deliver1 = Dijkstra(pizza_combo[j]->location, order_combo[a]->delivery_house, pizza_combo[j]->location->enter_deg, our_graph)->total_cost;
                            temp_cost += cost_to_deliver1;
                            // If the time taken to deliver the first one allows us to deliver it
                            // before its delivery period starts, we don't want this one.
                            if (GetCurrentTime() + temp_cost < order_combo[a]->delivery_period->start) {
                                // printf("Break 1: %f %f\n", temp_cost, order_combo[a]->delivery_period->start);
                                break;
                            }
                            // To deliver b
                            cost_to_deliver2 = Dijkstra(order_combo[a]->delivery_house, order_combo[b]->delivery_house, order_combo[a]->delivery_house->enter_deg, our_graph)->total_cost;
                            temp_cost += cost_to_deliver2;

                            if (GetCurrentTime() + temp_cost < order_combo[b]->delivery_period->start) {
                                // printf("Break 2: %f %f\n", temp_cost, order_combo[b]->delivery_period->start);
                                break;
                            }

                            if (temp_cost < lowest_cost) {
                                best_seq->should_cancel = FALSE;
                                lowest_cost = temp_cost;
                                
                                // Order 1 is the one we *pick* first
                                best_seq->order1 = order_combo[i];
                                best_seq->order2 = order_combo[j];

                                best_seq->pick1 = pizza_combo[i]->location;
                                best_seq->pick2 = pizza_combo[j]->location;
                                best_seq->deliver1 = order_combo[a]->delivery_house;
                                best_seq->deliver2 = order_combo[b]->delivery_house;
                            }
                            // And we go down the ugly slide now!
                        } // /
                    } //    /
                } //       /
            } //          /
        } //             /
    } //                /

    // ☻/  Phew. Didn't think I'd survive that huge a slide. Please don't let the bad man
    // /▌  write such terrible code again. Kthx.
    // /\                                         
    // 
    // 

    printf("Found best_seq for multiple orders\n");

    // We didn't find any combinations where we satisfied orders within their delivery
    // period, so we should definitely cancel this combo
    if (best_seq->should_cancel == TRUE) {
        return best_seq;
    }


    printf("Within delivery period\n");
    // If we get here, it means that best_seq delivers orders within
    // the delivery period.
    // Let's check if the combo delays our future orders
    best_seq->total_cost = lowest_cost;
    num_delayed_if_cancel = GetNumDelayed(best_seq->deliver2, GetCurrentTime() + lowest_cost, 1);
    printf("Within delivery period\n");
    num_delayed_if_deliver = GetNumDelayed(GetCurrentNode(), GetCurrentTime(), 1);
    printf("Within delivery period\n");
    if (num_delayed_if_cancel < num_delayed_if_deliver) {
        best_seq->should_cancel = TRUE;
    }

    printf("Should cancel: %d\n", best_seq->should_cancel);

    return best_seq;
}

// Finds the next regularly scheduled order
// If pos is 0, it finds the order that is immediately due
// If pos is 1, it finds the order due after that one
// And so on
Order *GetNextOrder(OrderList *timeline, int pos) {
    int i = 0;
    int num_found = 0;
    Order *current_order, *next_reg_order;
    next_reg_order = NULL;
    // //printf("Getting order %d\n", pos);
    for (i = 0; i < timeline->len; i++) {
        current_order = timeline->orders[i];
// ////        // printf("Checking %d %s\n", i, current_order->delivery_house->name);
        // If it hasn't already been canceled, delivered, or picked up, this is our next order
        if (current_order->state != 'c' && current_order->state != 'd' && current_order->state != 'h') {
            if (num_found == pos) {
                next_reg_order = current_order;
                break;
            }
            num_found++;
        }
    }
    if (next_reg_order == NULL) {
// ////        // printf("Order unavailable\n");
    }
    else {
        // //printf("Order #%d goes to %s at %d\n", num_found, next_reg_order->delivery_house->name, next_reg_order->order_time);
    }
    return next_reg_order;
}

Pizza *GetPizzaForOrder(Order *order) {
    int i = 0;
    Pizza *current_pizza;
    if (order == NULL) {
        return NULL;
    }

    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        // printf("Checking %c %c %c\n", current_pizza->colour, current_pizza->size, current_pizza->state);
        if (
            current_pizza->state != 'c' && current_pizza->state != 'd' &&
            current_pizza->size == order->size &&
            current_pizza->colour == order->colour
        ) {
            // //printf("Found pizza %c and %c\n", order->size, order->colour);
            return current_pizza;
        }
    }
    return NULL;
}

Order *GetOrderForPizza(Pizza *pizza) {
    int i = 0;
    Order *current_order;
    if (pizza == NULL) {
        return NULL;
    }

    for (i = 0; i < our_timeline->len; i++) {
        current_order = our_timeline->orders[i];
        // If the pizza / order haven't been delivered already, and
        // the colour & size match, they're compatible
        if (
            current_order->state != 'd' && pizza->state != 'd' &&
            current_order->size == pizza->size &&
            current_order->colour == pizza->colour
        ) {
            // //printf("Found pizza %c and %c\n", order->size, order->colour);
            return current_order;
        }
    }
    return NULL;
}

Pizza *GetPizzaAtNode(Node *node) {
    int i = 0;
    Pizza *current_pizza;
    if (node == NULL) {
        return NULL;
    }

    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        if (current_pizza->location == node) {
            return current_pizza;
        }
    }
    return NULL;
}

// Get all the pizzas that I can pick up in the current_period
// These are pizzas whose start times are past the start time of the current_period
// Example:
// If the bot is at H12, and has free time, it'll create a TimeBlock which
// starts at the current time + the time it'll need to go to the pizza counter
// This function will check which pizzas are available to be picked up in that time
// Whichever *are* available are returned in an OrderList
PizzaList *GetAvailablePizzas() {
    PizzaList *available_pizzas;
    Order *current_order;
    Pizza *current_pizza;
    Pizza *next_pizza = GetPizzaForOrder(GetNextOrder(our_timeline, 0));
    Node *cur_node = GetCurrentNode();
    int i = 0;
    // TODO: Discuss, experiment
    int threshold = 5;
    int cur_time = GetCurrentTime();
    float cost_to_pizza = INFINITY;
    available_pizzas = malloc(sizeof(PizzaList));
    available_pizzas->pizzas = malloc(MAX_ORDERS * sizeof(Pizza));

    for (i = 0; i < our_timeline->len; i++) {
        current_order = our_timeline->orders[i];
        current_pizza = GetPizzaForOrder(current_order);
        // Available pizzas is a consideration for extra pizzas, not regular ones, so we skip
        // over the one that's already on our list anyway
        if (current_pizza == next_pizza) {
            continue;
        }
        // If the current order has been found and it can be picked up within the time it takes us
        // to get there + a threshold (of waiting)
        // it's an available pizza!
        // Canceled pizzas are always available.
        cost_to_pizza = Dijkstra(cur_node, current_pizza->location, cur_node->enter_deg, GetGraph())->total_cost;
        if (
            cur_time + cost_to_pizza + threshold >= current_order->pickup_time &&
            current_order->state != 'd' &&
            current_pizza->found == TRUE
        ) {
            current_pizza->state = 'c';
            printf("\tSet state: %c %c\n", current_pizza->colour, current_pizza->size);
            InsertPizza(available_pizzas, current_pizza);
        }
    }
    return available_pizzas;
}

//                              -------                           ---------
char fake_block_sizes[] = {'l', 'n', 'n', 'l', 's', 'm', 'l', 's', 'n', 'n'};
char fake_colours[]     = {'r', 'r', 'r', 'g', 'r', 'b', 'b', 'r', 'r', 'r'};
int fake_i = 0;
int fake_len = 10;

// Our orders are:
// CreateOrder(our_timeline, 'r', 'l', 30, 'r', "H12");
// CreateOrder(our_timeline, 'g', 'l', 70, 'p', "H2");
// CreateOrder(our_timeline, 'r', 's', 50, 'r', "H4");
// CreateOrder(our_timeline, 'b', 'm', 89, 'r', "H4");
// CreateOrder(our_timeline, 'b', 'l', 0, 'r', "H6");
// CreateOrder(our_timeline, 'r', 's', 30, 'p', "H6");

// This is called when we're already next to a pizza, and we simply want
// to detect the pizza and save its details in our Order struct
// The state will change to 'f' to indicate that its been found
void DetectPizza() {
    int i = 0;
    Pizza *current_pizza;
    Pizza *conflicting_pizza;
    BotInfo *bot_info;
    int found = FALSE;
    // char block_size = 's'; // SharpGetBlockSize();
    // char colour = 'r'; // ColourGet();
    char block_size;
    char colour;
    block_size = fake_block_sizes[fake_i];
    colour = fake_colours[fake_i];
// //////    printf("%d\n", fake_i);
    fake_i++;
    if (fake_i >= fake_len) {
        fake_i--;
    }
    
    // Check the bot's current position
    // If we already have a pizza detected at this location, we're detecting nothing new
    // If we haven't, then correlate it to our_pizzas, and attach the current location to it
    // Also, use our current location to see which side the pizza was on
    // That information will be used in the future to decide which side is likelier to have pizzas
    // Now that we've found a new pizza, set the state to free, and let FreeTime
    // decide if we want to pick it up or not

    // TODO: Consider bad readings and rechecking?
    printf("Detected: %c and %c\n", colour, block_size);
    bot_info = GetBotInfo();
    total_pizzas++;
    // There is no pizza block at this location
    if (block_size == 'n') {
        // In this situation, we want to CreatePizza with details that no order will have
        // so that we know that we've already checked this location, but we also
        // never try to pick anything up from this location
        CreatePizza('n', 'n');
    }
    else {
        // We found an actual pizza, which might be worth picking up
        SetState('f');
    }

    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        if (
            (
                (current_pizza->colour == colour && current_pizza->size == block_size) ||
                (current_pizza->colour == 'n' && current_pizza->size == 'n')
            ) &&
            current_pizza->state == 'f' && // We don't want to override a pizza which has already been delivered
            current_pizza->found == FALSE
        ) {
            found = TRUE;
            // We just found pizza C at say c1
            // Previously, by our guesses: 
            // C->loc = c4;
            // F->loc = c1;
            // So we swap the guesses so that:
            // C->loc = c1
            // C->found = TRUE;
            // F->loc = c4
            conflicting_pizza = GetPizzaAtNode(bot_info->cur_position->cur_node);
            // current_pizza->location is our previous guess, not where it actually is
            // If we have less than 10 orders, and we haven't detected all the pizzas
            // we may not have a conflicting order at all
            if (conflicting_pizza) {
                conflicting_pizza->location = current_pizza->location;
                if (current_pizza->location == NULL) {
                    printf("AAAAAAAAAAAAAAAAAAA NULL NULL\n\n\n\n\n");
                }
            }
            current_pizza->location = bot_info->cur_position->cur_node;
            current_pizza->found = TRUE;
            // printf("Detected at %s, %c %c\n", current_pizza->location->name, colour, block_size);
            // There might be multiple pizzas with the same color and size so we want to make sure
            // we're filling in for one where the location was unfilled, and fill it only for that one
            // Hence the break
            break;
        }
    }

    // If this is an extra pizza or something, we want to add it to the list so we don't keep on
    // redetecting it
    if (found == FALSE) {
        CreatePizza(colour, block_size);
// //////        printf("Created: %c and %c\n", colour, block_size);
    }
}

// real_pizza: whether we're checking if there's a real pizza there or if we're checking if
// a location has been allocated
int IsPizzaAt(Node *test_node, int real_pizza) {
    int i = 0;
    int found = FALSE;
    Pizza *current_pizza;
    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        if (current_pizza->location == test_node && current_pizza->found == real_pizza) {
            found = TRUE;
// //////            printf("  Pizza at %s\n", test_node->name);
        }
    }
    return found;
}

// Consider looking for more pizzas
// This function will consider the cost of finding pizzas
// If the cost doesn't delay us for our regular order, it'll actually go search for pizzas
// Upon finding its first unknown pizza, it'll set the state to free (from within DetectPizza)
// If the cost does delay us, it'll set the state to 'b' so that NormalOperation can continue
// Returns whether it is finding pizzas or whether it decided to skip finding
// because it would delay us
// TRUE = Finding
// FALSE = Not finding
int FindPizzas() {
    BotInfo *bot_info;
    Graph *our_graph;
    PathStack *path_to_pizza, *path_to_left_pizza, *path_to_right_pizza;
    Node *target_pizza_node, *left_pizza_node, *right_pizza_node;
    Order *next_order = GetNextOrder(our_timeline, 0);
    Pizza *next_pizza = GetPizzaForOrder(next_order);
    int num_delayed_if_find = 0;
    int num_delayed_if_skip_find = 0;
    // TODO: Discuss threshold value
    // The threshold is here because if we have only 1 second of grace time to find a pizza
    // and then get to the next regular order, I think we shouldn't risk it
    // So we need an appropriate threshold for that.
    const int threshold = 5;
    float cost_to_find = 0;
    float cost_to_next_pizza = 0;
    bot_info = GetBotInfo();
    our_graph = GetGraph();
    
    if (total_pizzas == MAX_ORDERS) {
        // No more pizzas left to find, let's just get to delivering them
        // //printf("All pizzas found!\n");
        SetState('b');
        return FALSE;
    }

    // Now, we'll find the first node to the right and to the left
    // of the pizza counter. Whichever is closer is where we'll go

    // We go right right right
    right_pizza_node = GetFirstPToRight(TRUE);
    // If that one is already a known location in our list of pizzas, find the one to the right of that
    // Basically, find the first one to the right which is unknown
    path_to_right_pizza = Dijkstra(bot_info->cur_position->cur_node, right_pizza_node, bot_info->cur_position->cur_deg, our_graph);
    // printf("First unknown one to right is: %s, %f\n", right_pizza_node->name, path_to_right_pizza->total_cost);

    // We go left left left
    left_pizza_node = GetFirstPToLeft(TRUE);
    path_to_left_pizza = Dijkstra(bot_info->cur_position->cur_node, left_pizza_node, bot_info->cur_position->cur_deg, our_graph);
    // printf("First unknown one to left is: %s, %f\n", left_pizza_node->name, path_to_left_pizza->total_cost);

    path_to_pizza = path_to_left_pizza;
    target_pizza_node = left_pizza_node;
    // The path to the pizza is the one which is lower
    if (path_to_left_pizza->total_cost > path_to_right_pizza->total_cost) {
        path_to_pizza = path_to_right_pizza;
        target_pizza_node = right_pizza_node;
    }

    cost_to_find = path_to_pizza->total_cost;

    // How many future orders would be delayed if we went hunting for pizzas now
    // vs. how many would be delayed if we skipped finding pizzas
    
    // If we just had free time, and came here, it's because we didn't have any extra orders
    // So if finding pizzas delays us, we want to just go straight to normal operation
    
    // If we just had normal operation, and we didn't know where our next order was
    // we came here to look for it, but if looking for it is delaying us for the order
    // after that, we want to consider canceling the current one
    num_delayed_if_find = GetNumDelayed(target_pizza_node, GetCurrentTime() + cost_to_find, 0);
    num_delayed_if_skip_find = GetNumDelayed(GetCurrentNode(), GetCurrentTime(), 0);
    // Now we check if current time + cost + next_order_cost >= next_to_next_order.end_time
    // TODO: Consider effects of incorrect guesses? If we guessed that our next pizza is on the left
    // but we're on the right and it actually is on the right, we might cancel the current order
    // even though we don't need to.
    if (num_delayed_if_skip_find < num_delayed_if_find) {
        // TODO: Consider canceling current reg
        SetState('b');
        return FALSE;
    }
    else {
        // TODO:
        MoveBotToNode(target_pizza_node);
        // printf("--- Bot is at: %s\n", target_pizza_node->name);
        DetectPizza();
        return TRUE;
    }
}

TimeBlock *GetTimeReqForOrders(Order *order1, Order *order2) {
    TimeBlock *time_req;
    time_req = malloc(sizeof(TimeBlock));

    time_req->start = 0;
    time_req->end = 0;

    return time_req;
}

// This is called when we've got some free time, and want to consider doing the following:
// Finding more pizzas
// Picking an extra pizza up with the second arm (regular or preorder, doesn't matter)
// Picking an old, canceled pizza up
void FreeTimeDecision() {
    // We get the pizzas we can pick up in the next N seconds, where
    // N is the time taken for the bot to get to the pizza counter
    // Out of all of these, we'll eliminate pizzas based on this:
    // - If the cost of delivering this pizza + your next regular one
    // is enough to delay your next order or the one after that,
    // eliminate it
    // - If the cost mentioned above overlaps with a blocked period
    // of time, eliminate it
    // - If the next regular one is in the list, eliminate it

    // Whichever are left now can be picked up without any repurcussions,
    // so pick the one that's due the earliest up (TODO: should this be the one
    // that's closest in distance instead?)

    // If no pizzas are left, let's go ahead and find more pizzas *if time permits*
    // If there isn't time for that, set state to 'b'
    PizzaList *available_pizzas;
    Order *current_order, *next_order;
    // The pizzas for the corresponding orders above
    Pizza *current_pizza, *next_pizza;
    int i = 0;
    // Get pizzas that I can pick up by the time I can get to them
    available_pizzas = GetAvailablePizzas();
    next_order = GetNextOrder(our_timeline, 0);
    next_pizza = GetPizzaForOrder(next_order);
    // Every OrderList is sorted by due time already, so the first one we find that
    // ConsiderCancel thinks won't delay orders is the one we consider delivering
    for (i = 0; i < available_pizzas->len; i++) {
        // If there is a pizza available for this order
        // and the order isn't our next_reg_order anyway
        // and doesn't overlap with our blocked time
        // and doesn't delay next orders
        // LET'S DO THIS!
        current_pizza = available_pizzas->pizzas[i];
        // We've already got our list of considered pizzas, so we can mark them
        // as free to be considered again now.
        current_pizza->state = 'f';
        printf("\tReset state: %c %c\n", current_pizza->colour, current_pizza->size);
    }

    for (i = 0; i < available_pizzas->len; i++) {
        // If the pizza's location isn't known, we don't want to waste time
        // with extra orders. We need to find this pizza!
        // TODO: Special case for canceled orders?
        if (next_pizza != NULL && next_pizza->location == NULL) {
            SetState('b');
            return;
        }
        current_order = GetOrderForPizza(current_pizza);

        printf("Free time, considering combos\n");
        Display(current_order);
        // printf("Debug %d: %d %d", next_extra_order == NULL, current_pizza == NULL, ConsiderCancel(next_order, current_order)->should_cancel);
        if (current_pizza != NULL) {
            // printf(" %d", current_pizza->location == NULL);
        }
        // printf("\n");

        if (
            current_pizza->found == TRUE &&
            current_pizza != NULL && current_pizza->location != NULL &&
            current_order != next_order &&
            // !CheckOverlap(next_required_period, GetTimeReqForOrders(next_order, current_order)) &&
            ConsiderCancel(next_order, current_order)->should_cancel == FALSE
        ) {
            next_extra_order = current_order;
            printf("\tExtra order: %s at %d, found %d\n", next_extra_order->delivery_house->name, next_extra_order->order_time, current_pizza->found);
            break;
        }
    }
    // If no pizzas were found that satisfied the conditions above, we should consider looking for
    // more pizzas
    if (next_extra_order == NULL) {
        // FindPizzas takes care of considering cost and everything.
        // If it thinks it'll delay us to find more pizzas, it'll change the state to 'b'
        // printf("Finding pizzas...\n");
        FindPizzas();
    }
    else {
        SetState('b');
    }
// ////    printf("\tFree time over\n");
}

void DeliverPizzas(DeliverySequence *cur_sequence) {
    // sleep(cur_sequence/5.0->total_cost);
    // GetBotInfo()->cur_position->cur_node = cur_sequence->deliver2 == NULL ? cur_sequence->deliver1 : cur_sequence->deliver2;
    int i = 0;
    Pizza *delivered_pizza;

    // Node *locations[] = {cur_sequence->pick1, cur_sequence->pick2, cur_sequence->deliver1, cur_sequence->deliver2};
    // // printf("Starting delivery\n");
    // for (i = 0; i < 4; i++) {
    //     if (locations[i] != NULL) {
    //         delivered_pizza = GetPizzaAtNode(locations[i]);
    //         if (delivered_pizza) {
    //             delivered_pizza->state = 'c';
    //             // TODO: Make the location be one of the deposition zones
    //             delivered_pizza->location = NULL;
    //         }
    //         MoveBotToNode(locations[i]);
    //     }
    // }
    printf("Delivering\n");
    if (cur_sequence->guess1 == TRUE) {
        // FindSpecificPizza(cur_sequence->order1);
    }
    if (cur_sequence->pick1 != NULL) {
        MoveBotToNode(cur_sequence->pick1);
        delivered_pizza = GetPizzaAtNode(cur_sequence->pick1);
        delivered_pizza->state = 'd';
        delivered_pizza->location = NULL;
        printf("\t\tP-loc null: %c %c\n\n", delivered_pizza->colour, delivered_pizza->size);
        if (cur_sequence->order1->pickup_time > GetCurrentTime()) {
            // TODO: Consider this as free time if possible?
            printf("Reached early. Waiting %d %d %d\n", cur_sequence->order1->pickup_time - GetCurrentTime(), cur_sequence->order1->pickup_time, GetCurrentTime());
            sleep(cur_sequence->order1->pickup_time - GetCurrentTime());
        }
        // PickPizza();
    }

    if (cur_sequence->pick2 != NULL) {
        MoveBotToNode(cur_sequence->pick2);
        delivered_pizza = GetPizzaAtNode(cur_sequence->pick1);
        delivered_pizza->state = 'd';
        delivered_pizza->location = NULL;
        printf("\t\tP-loc null: %c %c\n\n", delivered_pizza->colour, delivered_pizza->size);
        if (cur_sequence->order2->pickup_time > GetCurrentTime()) {
            // TODO: Consider this as free time if possible?
            printf("Reached early. Waiting %d %d %d\n", cur_sequence->order2->pickup_time - GetCurrentTime(), cur_sequence->order2->pickup_time, GetCurrentTime());
            sleep(cur_sequence->order2->pickup_time - GetCurrentTime());
        }
        // PickPizza();
    }

    if (cur_sequence->deliver1 != NULL) {
        MoveBotToNode(cur_sequence->deliver1);
        // DropPizza();
    }

    if (cur_sequence->deliver2 != NULL) {
        MoveBotToNode(cur_sequence->deliver2);
        // DropPizza();
    }

    printf("Delivered pizzas! ");
    if (cur_sequence->order1 != NULL) {
        printf("%s at %d, ", cur_sequence->order1->delivery_house->name, cur_sequence->order1->order_time);
        orders_completed++;
        cur_sequence->order1->state = 'd';
    }
    if (cur_sequence->order2 != NULL) {
        printf("%s at %d, ", cur_sequence->order2->delivery_house->name, cur_sequence->order2->order_time);
        orders_completed++;
        cur_sequence->order2->state = 'd';
    }
    printf("\n");

    next_extra_order = NULL;
    SetState('f');
}

void NormalOperation() {
    // So let's go pick our next regular order & next extra order
    // Then figure out which to deliver first
    // And deliver it
    // Then deliver the next one
    // When both arms are empty, state will be set to free
    // And so the cycle continues

    /*
    Bored? Here's a song to sing:
    It's the Circle of Life
    And it moves us all
    Through despair and hope
    Through faith and love

    Till we find our place
    On the path unwinding
    In the Circle
    The Circle of Life
    */
    DeliverySequence *cur_sequence;
    Order *next_reg_order;
    Pizza *next_reg_pizza;
    int no_time_to_find = FALSE;

    next_reg_order = GetNextOrder(our_timeline, 0);
    // If we've delivered all the regular orders already, we can consider the canceled pizzas
    // as our regular orders.
    if (next_reg_order == NULL && next_extra_order != NULL) {
        next_reg_order = next_extra_order;
        next_extra_order = NULL;
        // //printf("Shuffling extra and reg around\n");
    }
    printf("Reg: ");
    Display(next_reg_order);
    printf("Extra: ");
    Display(next_extra_order);
    printf("\n");
    next_reg_pizza = GetPizzaForOrder(next_reg_order);
    // TODO: Special case for canceled orders
    if (next_reg_pizza == NULL || next_reg_pizza->location == NULL) {
        // If the location of our regular order is unknown, then we want to find it
        // without considering the cost of delaying it
        // TODO: If FindPizzas thinks we should cancel, cancel?
        no_time_to_find = FindPizzas();
        if (no_time_to_find) {
            // Finding the current pizza seems like it'll delay us,
            // so lets skip this order
            next_reg_order->state = 'c';
            // Now that we've canceled it, maybe we have extra time to find pizzas or pick up
            // extra pizzas, so we consider it free time
            SetState('f');
            return;
        }
    }
    // next_extra_order might be NULL, but that's okay, because ConsiderCancel
    // can deal with it.
    cur_sequence = ConsiderCancel(next_reg_order, next_extra_order);
    // //printf("Should cancel: %d cur_time: %d\n", cur_sequence->should_cancel, GetCurrentTime());

    // If things work as expected, ConsiderCancel will never say we should cancel
    // when we're considering next_reg and next_extra, because we've previously
    // already considered that cost and eliminated all that would delay us

    // So if things work, it'll tell us to cancel only regular orders

    // If it said we should cancel, lets mark the order as canceled
    // and move on, because there are plenty of pizzas in the sea (pizza counter, sea,
    // what's the difference? Potato potato. Doesn't translate to text, sadly.)

    // Since our next order has changed now, we should consider doing some
    // extra work in our free time!
    // If we're in a tight situation, the FreeTime function will recognize
    // that and send us back here for our regularly scheduled delivery
    if (cur_sequence->should_cancel == TRUE) {
        printf("Canceling order to %s at %d %c\n", next_reg_order->delivery_house->name, next_reg_order->order_time, next_reg_order->order_type);
        next_reg_order->state = 'c';
        return;
    }
    // ConsiderCancel thinks we can deliver it in time, so lets do it!
    // When we're done, we want to go into "free state" mode
    DeliverPizzas(cur_sequence);

}

void TimelineControl() {
    SetState('f');
    while (1) {
        // both functions below will be blocking
        // this means that they will not return until they've completed their task
        // free will return only when it's decided to pick up the extra pizza or called find_pizzas
        // or it has set the state to busy
        // reg will only return when it has picked up and delivered the pizzas that it has to
        // or when it's canceled a delivery
        printf("\t\tTotal time: %d\n", GetCurrentTime());
        printf("\t\tTotal orders: %d\n", orders_completed);
        printf("\t\tTotal pizzas: %d\n", total_pizzas);
        if (orders_completed == total_orders) {
            printf("All done!\n\n\n");
            return;
        }
        if (GetState() == 'f') {
            printf("\tFree time!\n");
            FreeTimeDecision(); // this will set the state to busy if it didn't find any pizzas to deliver
        }
        else {
            printf("\tNormal operation!\n");
            NormalOperation(); // this will set the state to free when the delivery is done
        }
        sleep(1/5.0);
    }
}

void Display(Order *current_order) {
    if (current_order == NULL) {
        return;
    }
    printf("colour: %c, ", current_order->colour);
    printf("size: %c, ", current_order->size);
    printf("time: %d, ", current_order->order_time);
    printf("type: %c, ", current_order->order_type);
    printf("state: %c, ", current_order->state);
    printf("house: %s\n", current_order->delivery_house->name);
   // //// printf("pickup point: %s\n", current_order->pickup_point);
}
