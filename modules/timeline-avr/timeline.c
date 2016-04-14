#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// #include <unistd.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "timeline.h"
#include "../map/map.h"
#include "../dijkstra/dijkstra.h"
#include "../bot_memory/bot_memory.h"
#include "../timer/timer.h"
#include "../custom_delay/custom_delay.h"
#include "../hardware_control/hardware_control.h"
#include "../arm_control/arm_control.h"
#include "../move_bot/move_bot.h"
#include "../buzzer/buzzer.h"
#include "../rgb_led/rgb_led.h"
#include "../color_sensor/color_sensor.h"
#include "../sharp_sensor/sharp_sensor.h"

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
char total_pizzas = 0;

char total_orders = 0;
char orders_completed = 0;

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

// This may be NULL often. It'll be the pizza we pick up using our extra arm when our FreeTime function
// thinks it's a good idea.
// To clarify, FreeTime will only set this variable to an order.
// The NormalOperation function is the one that will actually pick it up
// and deliver it.
Order *next_extra_order;

// We only have 10 pizza pick up points, and since the rules state that no pizzas will be added
// or removed *during* a run, we can assume that the max orders we'll get is 10
const char MAX_ORDERS = 10;
const char GUARANTEED_PERIOD = 50;

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
    unsigned char i = 0;
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
    // printf("Created pizza %c %c", colour, size);

    // Insert into our array of pizzas
    InsertPizza(our_pizzas, pizza);

    return pizza;
}

void CreateOrder(
        OrderList *timeline,
        char colour,
        char size,
        short int order_time, // A byte-long int
        char order_type,
        char *delivery_house_name
    ) {

    Order *new_order;

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
    // printf("%d: %s", i, delivery_house_name);
}

void InitTimeline() {
    our_timeline = malloc(sizeof(OrderList));
    our_timeline->orders = malloc(MAX_ORDERS * sizeof(Order));
    our_timeline->len = 0;

    // Pizza list is twice as long for the worst case situation:
    // All ordered pizzas are missing
    // Then our_pizzas will be filled with 10 pizzas according to our
    // orders
    // And 10 "empty pizzas" which it'll detect
    our_pizzas = malloc(sizeof(PizzaList));
    our_pizzas->pizzas = malloc(2 * MAX_ORDERS * sizeof(Pizza));
    our_pizzas->len = 0;

    PIZZA_COUNTER_NODE = GetNodeByName("r1"); // r1 is always going to be our "pizza counter"


    // For reference:
    // CreateOrder(OrderList *timeline, char colour, char size, int order_time, char order_type, char *delivery_house_name)
    // CreateOrder(our_timeline, 'r', 'l', 30, 'r', "H12");
    // CreateOrder(our_timeline, 'g', 'l', 70, 'p', "H2");
    // CreateOrder(our_timeline, 'r', 's', 50, 'r', "H4");
    // CreateOrder(our_timeline, 'b', 'm', 89, 'r', "H4");
    // CreateOrder(our_timeline, 'b', 'l', 0, 'r', "H6");
    // CreateOrder(our_timeline, 'r', 's', 30, 'p', "H6");

    // CreateOrder(our_timeline, 'b', 'l', 30, 'r', "H6");
    // CreateOrder(our_timeline, 'r', 's', 30, 'p', "H6");
    // CreateOrder(our_timeline, 'r', 'l', 70, 'r', "H12");
    // CreateOrder(our_timeline, 'r', 's', 110, 'r', "H4");
    // CreateOrder(our_timeline, 'g', 'l', 150, 'p', "H2");
    // CreateOrder(our_timeline, 'b', 'm', 220, 'r', "H4");
    
    CreateOrder(our_timeline, 'r', 'l', 40, 'r', "H1");
    CreateOrder(our_timeline, 'b', 's', 100, 'r', "H8");
    CreateOrder(our_timeline, 'g', 'm', 115, 'p', "H2");
    CreateOrder(our_timeline, 'r', 's', 200, 'p', "H11");
    CreateOrder(our_timeline, 'r', 'l', 250, 'r', "H12");
    CreateOrder(our_timeline, 'g', 's', 350, 'p', "H5");
    CreateOrder(our_timeline, 'b', 'm', 350, 'p', "H5");
}

OrderList *GetTimeline() {
    return our_timeline;
}

PizzaList *GetPizzas() {
    return our_pizzas;
}

void MissingOrderBeep() {
    // Beep the buzzer indicating a missing pizza
    // printf("Order missing!");
    BuzzerOn();
    if (RegisterCallback(BuzzerOff, 1) == 0) {
        _delay_ms(1000);
        BuzzerOff();
    };
    BuzzerOn();
    if (RegisterCallback(BuzzerOff, 1) == 0) {
        _delay_ms(1000);
        BuzzerOff();
    };
}

char GetNumDelayed(Node *source_node, short int start_time, char order_num) {
    char delayed = 0;
    PathStack *path_to_pick;
    PathStack *path_to_deliver;
    float cost_to_pick = INFINITY;
    float cost_to_deliver = INFINITY;
    float total_cost = INFINITY;
    char cur_order_num = order_num;
    Graph *our_graph = GetGraph();
    Order *next_order;
    Pizza *order_pizza;
    PizzaList *used_pizzas;
    unsigned char i = 0;
    used_pizzas = malloc(sizeof(*used_pizzas));
    used_pizzas->pizzas = malloc(MAX_ORDERS * sizeof(Pizza *));
    used_pizzas->len = 0;
    path_to_pick = path_to_deliver = NULL;

    next_order = GetNextOrder(our_timeline, cur_order_num);
    order_pizza = GetPizzaForOrder(next_order);

    while (next_order != NULL) {
        // We set the state to "considering" so that when the function is called recursively,
        // the next order doesn't try to access the same pizza
        order_pizza = GetPizzaForOrder(next_order);
        if (order_pizza != NULL) {
            order_pizza->state = 'c';
            InsertPizza(used_pizzas, order_pizza);
        }
        else {
            // printf("OrderPizza NULL! %c %c", next_order->colour, next_order->size);
        }

        // We don't care about using guessed pizza locations, as long as
        // the calls compare in a similar manner.
        path_to_pick = Dijkstra(source_node, order_pizza->location, source_node->enter_deg, our_graph);
        cost_to_pick = path_to_pick->total_cost;

        // If we get there before we can pick it up, our cost to pick is actually higher
        if (start_time + cost_to_pick < next_order->pickup_time) {
            cost_to_pick = next_order->pickup_time - start_time;
        }
        path_to_deliver = Dijkstra(order_pizza->location, next_order->delivery_house, order_pizza->location->enter_deg, our_graph);
        cost_to_deliver = path_to_deliver->total_cost;

        total_cost = start_time + cost_to_pick + cost_to_deliver;
        // If we reach after the delivery period ends, we've delayed this order
        if (total_cost > next_order->delivery_period->end) {
            delayed += 1;
        }
        start_time = total_cost;
        source_node = order_pizza->location;
        cur_order_num++;

        next_order = GetNextOrder(our_timeline, cur_order_num);
        DijkstraFree(path_to_pick);
        DijkstraFree(path_to_deliver);
    }

    for (i = 0; i < used_pizzas->len; i++) {
        used_pizzas->pizzas[i]->state = 'f';
    }

    free(used_pizzas->pizzas);
    free(used_pizzas);

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
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char a = 0;
    unsigned char b = 0;
    const char num_combos = 2;
    // An array of order pointers
    Order **order_combo;
    Pizza **pizza_combo;
    Order *single_order;
    Pizza *single_pizza;
    Node *second_delivery_loc;
    order_combo = malloc(2 * sizeof(Order *));
    order_combo[0] = order1;
    order_combo[1] = order2;
    pizza_combo = malloc(2 * sizeof(Pizza *));

    BotInfo *bot_info = GetBotInfo();
    Graph *our_graph = GetGraph();
    DeliverySequence *best_seq;
    best_seq = malloc(sizeof(DeliverySequence));
    best_seq->order_combo = order_combo;
    best_seq->pizza_combo = pizza_combo;
    best_seq->should_cancel = FALSE;

    // Number of regular orders delayed if deliver the current order vs. if we cancel it
    char num_delayed_if_deliver = 0;
    char num_delayed_if_cancel = 0;

    // printf("ConsiderCancel %d %d", order1 == NULL, order2 == NULL);

    float lowest_cost = INFINITY;
    float temp_cost = INFINITY;
    float cost_to_pick1 = INFINITY;
    float cost_to_pick2 = INFINITY;
    float cost_to_deliver1 = INFINITY;
    float cost_to_deliver2 = INFINITY;
    PathStack *path_to_pick1;
    PathStack *path_to_pick2;
    PathStack *path_to_deliver1;
    PathStack *path_to_deliver2;
    path_to_pick1 = NULL;
    path_to_pick2 = NULL;
    path_to_deliver1 = NULL;
    path_to_deliver2 = NULL;

    // If either order is null, then there's only one way to deliver the remaining order
    // Go pick it up, deliver it
    if (order1 == NULL || order2 == NULL) {
        if (order2 == order1) {
            // printf("Both orders are NULL, doofus.");
            free(order_combo);
            free(pizza_combo);
            free(best_seq);
            return NULL;
        }
        // printf("Single order");
        single_order = (order1 == NULL) ? order2 : order1;
        single_pizza = GetPizzaForOrder(single_order);
        // Cost to go pick the pizza up
        path_to_pick1 = Dijkstra(bot_info->cur_position->cur_node, single_pizza->location, bot_info->cur_position->cur_deg, our_graph);
        cost_to_pick1 = path_to_pick1->total_cost;
        // If we get there before we're allowed to pick the pizza up
        // then we want to consider the waiting time as well
        if (GetCurrentTime() + cost_to_pick1 < single_order->pickup_time) {
            cost_to_pick1 = single_order->pickup_time - GetCurrentTime();
        }
        // Plus cost of delivering pizza
        path_to_deliver1 = Dijkstra(single_pizza->location, single_order->delivery_house, single_pizza->location->enter_deg, our_graph);
        cost_to_deliver1 = path_to_deliver1->total_cost;
        temp_cost = cost_to_pick1 + cost_to_deliver1;

        best_seq->order_combo[0] = single_order;
        best_seq->pizza_combo[0] = single_pizza;
        best_seq->order_combo[1] = NULL;
        best_seq->pizza_combo[1] = NULL;
        
        best_seq->pick1 = 0;
        best_seq->deliver1 = 0;
        best_seq->pick2 = 1;
        best_seq->deliver2 = 1;
        // best_seq->pick1 = single_pizza->location;
        // best_seq->deliver1 = single_order->delivery_house;
        best_seq->total_cost = temp_cost;

        // printf("House null: %d", single_order->delivery_house == NULL);
        num_delayed_if_deliver = GetNumDelayed(single_order->delivery_house, GetCurrentTime() + temp_cost, 1);
        num_delayed_if_cancel = GetNumDelayed(GetCurrentNode(), GetCurrentTime(), 1);
        // If we delay fewer pizzas later by canceling, then lets cancel
        // printf("Delayed: %d %d", num_delayed_if_cancel, num_delayed_if_deliver);
        if (num_delayed_if_cancel < num_delayed_if_deliver) {
            best_seq->should_cancel = TRUE;
            // printf("Should cancel because future delays");
        }
        DijkstraFree(path_to_pick1);
        DijkstraFree(path_to_deliver1);
        return best_seq;
    }


    // printf("Getting pizzas for orders");
    // Display(order1);
    // Display(order2);
    pizza_combo[0] = GetPizzaForOrder(order1);
    pizza_combo[1] = GetPizzaForOrder(order2);

    // printf("*** Multiple orders %d, %d, %d, %d", pizza_combo[0] == NULL, pizza_combo[1] == NULL, order_combo[0] == NULL, order_combo[1] == NULL);

    // printf("%d", pizza_combo[0]->location == NULL);


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
                        // // //// printf("aaaa%d, %d - %d, %d", i, j, a, b);
                        if (
                            a != b &&
                            pizza_combo[i]->location != NULL &&
                            pizza_combo[j]->location != NULL &&
                            order_combo[a]->delivery_house != NULL &&
                            order_combo[b]->delivery_house != NULL
                        ) {
                            // printf("%d, %d - %d, %d", i, j, a, b);
                            // Get cost to pick i
                            // If we get there before we can pick it up, we have to wait until
                            // it's time to pick them up
                            path_to_pick1 = Dijkstra(bot_info->cur_position->cur_node, pizza_combo[i]->location, bot_info->cur_position->cur_deg, our_graph);
                            cost_to_pick1 = path_to_pick1->total_cost;
                            if (GetCurrentTime() + cost_to_pick1 < order_combo[i]->pickup_time) {
                                cost_to_pick1 = GetCurrentTime() - order_combo[i]->pickup_time;
                            }
                            temp_cost = cost_to_pick1;

                            // To pick j
                            // If we get there before we can pick it up, we have to wait until
                            // it's time to pick them up
                            path_to_pick2 = Dijkstra(pizza_combo[i]->location, pizza_combo[j]->location, pizza_combo[i]->location->enter_deg, our_graph);
                            cost_to_pick2 = path_to_pick2->total_cost;
                            if (GetCurrentTime() + cost_to_pick1 + cost_to_pick2 < order_combo[j]->pickup_time) {
                                cost_to_pick2 = GetCurrentTime() - order_combo[j]->pickup_time;
                            }
                            temp_cost += cost_to_pick2;

                            // To deliver a
                            path_to_deliver1 = Dijkstra(pizza_combo[j]->location, order_combo[a]->delivery_house, pizza_combo[j]->location->enter_deg, our_graph);
                            cost_to_deliver1 = path_to_deliver1->total_cost;

                            // If the time taken to deliver the first one allows us to deliver it
                            // before its delivery period starts, we don't want this one.
                            if (GetCurrentTime() + temp_cost + cost_to_deliver1 < order_combo[a]->delivery_period->start) {
                                cost_to_deliver1 = order_combo[a]->delivery_period->start - GetCurrentTime() - temp_cost;
                                // printf("Cd1 %f", cost_to_deliver1);
                                // break;
                            }
                            temp_cost += cost_to_deliver1;

                            // To deliver b
                            path_to_deliver2 = Dijkstra(order_combo[a]->delivery_house, order_combo[b]->delivery_house, order_combo[a]->delivery_house->enter_deg, our_graph);
                            cost_to_deliver2 = path_to_deliver2->total_cost;

                            if (GetCurrentTime() + temp_cost + cost_to_deliver2 < order_combo[b]->delivery_period->start) {
                                cost_to_deliver2 = order_combo[b]->delivery_period->start - GetCurrentTime() - temp_cost;
                                // printf("Cd2 %f", cost_to_deliver2);
                                // break;
                            }
                            temp_cost += cost_to_deliver2;

                            if (temp_cost < lowest_cost) {
                                best_seq->should_cancel = FALSE;
                                lowest_cost = temp_cost;
                                
                                best_seq->pick1 = i;
                                best_seq->pick2 = j;
                                best_seq->deliver1 = a;
                                best_seq->deliver2 = b;

                                // Order 1 is the one we *pick* first
                                // best_seq->order1 = order_combo[i];
                                // best_seq->order2 = order_combo[j];

                                // best_seq->pick1 = pizza_combo[i]->location;
                                // best_seq->pick2 = pizza_combo[j]->location;
                                // best_seq->deliver1 = order_combo[a]->delivery_house;
                                // best_seq->deliver2 = order_combo[b]->delivery_house;
                            }

                            DijkstraFree(path_to_pick1);
                            DijkstraFree(path_to_pick2);
                            DijkstraFree(path_to_deliver1);
                            DijkstraFree(path_to_deliver2);

                            // And we go down the ugly slide now!
                        } // /
                    } //    /
                } //       /
            } //          /
        } //             /
    } //                /

    // ☻/  Phew. Didn't think I'd survive that huge a slide. Please don't let the bad man
    // /▌  write such terrible code again. Kthx.
    // /|                                         
    // 
    // 


    // printf("Found best_seq for multiple orders");

    // We didn't find any combinations where we satisfied orders within their delivery
    // period, so we should definitely cancel this combo
    if (best_seq->should_cancel == TRUE) {
        return best_seq;
    }

    // If we get here, it means that best_seq delivers orders within
    // the delivery period.
    // Let's check if the combo delays our future orders
    best_seq->total_cost = lowest_cost;
    second_delivery_loc = best_seq->order_combo[best_seq->deliver2]->delivery_house;
    num_delayed_if_deliver = GetNumDelayed(second_delivery_loc, GetCurrentTime() + lowest_cost, 1);
    num_delayed_if_cancel = GetNumDelayed(GetCurrentNode(), GetCurrentTime(), 1);
    // printf("Num del: %d %d", num_delayed_if_cancel, num_delayed_if_deliver);
    if (num_delayed_if_cancel < num_delayed_if_deliver) {
        best_seq->should_cancel = TRUE;
    }

    return best_seq;
}

// Finds the next regularly scheduled order
// If pos is 0, it finds the order that is immediately due
// If pos is 1, it finds the order due after that one
// And so on
Order *GetNextOrder(OrderList *timeline, char pos) {
    unsigned char i = 0;
    char num_found = 0;
    Order *current_order, *next_reg_order;
    next_reg_order = NULL;
    // printf("Getting order %d", pos);
    for (i = 0; i < timeline->len; i++) {
        current_order = timeline->orders[i];
// // ////        // printf("Checking %d %s", i, current_order->delivery_house->name);
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
// // ////        // printf("Order unavailable");
    }
    else {
        // printf("Order #%d goes to %s at %d", num_found, next_reg_order->delivery_house->name, next_reg_order->order_time);
    }
    return next_reg_order;
}

Pizza *GetPizzaForOrder(Order *order) {
    unsigned char i = 0;
    Pizza *current_pizza;
    if (order == NULL) {
        return NULL;
    }

    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        // printf("Checking %c %c %c", current_pizza->colour, current_pizza->size, current_pizza->state);
        if (
            current_pizza->state != 'c' && current_pizza->state != 'd' &&
            current_pizza->size == order->size &&
            current_pizza->colour == order->colour
        ) {
            // printf("Found pizza %c and %c", order->size, order->colour);
            return current_pizza;
        }
    }
    return NULL;
}

Order *GetOrderForPizza(Pizza *pizza) {
    unsigned char i = 0;
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
            // printf("Found pizza %c and %c", order->size, order->colour);
            return current_order;
        }
    }
    return NULL;
}

Pizza *GetPizzaAtNode(Node *node) {
    unsigned char i = 0;
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
    PathStack *path_to_pizza;
    Order *current_order;
    Pizza *current_pizza;
    Pizza *next_pizza = GetPizzaForOrder(GetNextOrder(our_timeline, 0));
    Node *cur_node = GetCurrentNode();
    unsigned char i = 0;
    // TODO: Discuss, experiment
    char threshold = 5;
    short int cur_time = GetCurrentTime();
    float cost_to_pizza = INFINITY;
    available_pizzas = malloc(sizeof(PizzaList));
    available_pizzas->pizzas = malloc(MAX_ORDERS * sizeof(Pizza*));
    available_pizzas->len = 0;
    path_to_pizza = NULL;

    // printf("Getting available pizzas");

    for (i = 0; i < our_timeline->len; i++) {
        current_order = our_timeline->orders[i];
        if (current_order->state == 'd') {
            // printf("Skipping delivered order");
            continue;
        }
        current_pizza = GetPizzaForOrder(current_order);
        // Available pizzas is a consideration for extra pizzas, not regular ones, so we skip
        // over the one that's already on our list anyway
        if (current_pizza == next_pizza) {
            // printf("Skipped same");
            continue;
        }
        // If the current order has been found and it can be picked up within the time it takes us
        // to get there + a threshold (of waiting)
        // it's an available pizza!
        // Canceled pizzas are always available.
        path_to_pizza = Dijkstra(cur_node, current_pizza->location, cur_node->enter_deg, GetGraph());
        cost_to_pizza = path_to_pizza->total_cost;

        // printf("%d %d %d", (int) (cur_time + cost_to_pizza + threshold), current_order->pickup_time, current_pizza->found);
        if (
            cur_time + cost_to_pizza + threshold >= current_order->pickup_time &&
            current_order->state != 'd' &&
            current_pizza->found == TRUE
        ) {
            current_pizza->state = 'c';
            // printf("Set state: %c %c", current_pizza->colour, current_pizza->size);
            InsertPizza(available_pizzas, current_pizza);
        }
        DijkstraFree(path_to_pizza);
    }
    return available_pizzas;
}

//                              -------                           ---------
// char fake_colours[]     = {'g', 'b', 'g', 'r', 'b', 'n', 'r', 'r', 'n', 'g'};
// char fake_block_sizes[] = {'m', 'm', 's', 'm', 's', 'n', 'l', 's', 'n', 'l'};
// unsigned char fake_i = 0;
// unsigned char fake_len = 10;

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
    unsigned char i = 0;
    Pizza *current_pizza;
    Pizza *conflicting_pizza;
    float deg_to_pizza = -90;
    BotInfo *bot_info;
    char found = FALSE;
    // char block_size = 's'; // SharpGetBlockSize();
    // char colour = 'r'; // ColourGet();
    char block_size;
    char colour;
    bot_info = GetBotInfo();
//     block_size = fake_block_sizes[fake_i];
//     colour = fake_colours[fake_i];
// // // //////    printf("%d", fake_i);
//     fake_i++;
//     if (fake_i >= fake_len) {
//         fake_i--;
//     }
    RotateBot((int) GetShortestDeg(deg_to_pizza - (bot_info->cur_position->cur_deg + bot_info->sensor_angle)), FALSE);
    block_size = SharpGetBlockType();
    colour = GetPizzaColor();
    if (block_size == 'n') {
        block_size = SharpGetBlockType();
    }
    
    // Check the bot's current position
    // If we already have a pizza detected at this location, we're detecting nothing new
    // If we haven't, then correlate it to our_pizzas, and attach the current location to it
    // Also, use our current location to see which side the pizza was on
    // That information will be used in the future to decide which side is likelier to have pizzas
    // Now that we've found a new pizza, set the state to free, and let FreeTime
    // decide if we want to pick it up or not

    // TODO: Consider bad readings and rechecking?
    // sleep(1);
    printf("Detected: %c, %c", colour, block_size);
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
                // if (current_pizza->location == NULL) {
                // //     printf("AAAAAAAAAAAAAAAAAAA NULL NULL");
                // }
            }
            current_pizza->location = bot_info->cur_position->cur_node;
            current_pizza->found = TRUE;
            // printf("Detected at %s, %c %c", current_pizza->location->name, colour, block_size);
            // There might be multiple pizzas with the same color and size so we want to make sure
            // we're filling in for one where the location was unfilled, and fill it only for that one
            // Hence the break
            break;
        }
    }

    // If this is an extra pizza or something, we want to add it to the list so we don't keep on
    // redetecting it
    if (found == FALSE) {
        // Create the pizza and mark it as found
        current_pizza = CreatePizza(colour, block_size);
        conflicting_pizza = GetPizzaAtNode(bot_info->cur_position->cur_node);
        if (conflicting_pizza) {
            conflicting_pizza->location = current_pizza->location;
            // if (current_pizza->location == NULL) {
            // //     printf("AAAAAAAAAAAAAAAAAAA NULL NULL");
            // }
        }
        current_pizza->location = bot_info->cur_position->cur_node;
        current_pizza->found = TRUE;
    }
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
char FindPizzas() {
    BotInfo *bot_info;
    Graph *our_graph;
    PathStack *path_to_pizza, *path_to_left_pizza, *path_to_right_pizza;
    Node *target_pizza_node, *left_pizza_node, *right_pizza_node;
    char num_delayed_if_find = 0;
    char num_delayed_if_skip_find = 0;
    float cost_to_find = 0;
    bot_info = GetBotInfo();
    our_graph = GetGraph();
    
    if (total_pizzas == MAX_ORDERS) {
        // No more pizzas left to find, let's just get to delivering them
        // printf("All pizzas found!");
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
    // printf("First unknown one to right is: %s, %f", right_pizza_node->name, path_to_right_pizza->total_cost);

    // We go left left left
    left_pizza_node = GetFirstPToLeft(TRUE);
    path_to_left_pizza = Dijkstra(bot_info->cur_position->cur_node, left_pizza_node, bot_info->cur_position->cur_deg, our_graph);
    // printf("First unknown one to left is: %s, %f", left_pizza_node->name, path_to_left_pizza->total_cost);

    path_to_pizza = path_to_left_pizza;
    target_pizza_node = left_pizza_node;
    // The path to the pizza is the one which is lower
    if (path_to_left_pizza->total_cost > path_to_right_pizza->total_cost) {
        path_to_pizza = path_to_right_pizza;
        target_pizza_node = right_pizza_node;
    }

    cost_to_find = path_to_pizza->total_cost;
    // printf("Targ p: %s %d", target_pizza_node->name, 1);//GetPizzaAtNode(target_pizza_node)->found);

    DijkstraFree(path_to_right_pizza);
    DijkstraFree(path_to_left_pizza);
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
        // printf("Skipping find");
        SetState('b');
        return FALSE;
    }
    else {
        // TODO:
        MoveBotToNode(target_pizza_node);
        // printf("--- Bot is at: %s", target_pizza_node->name);
        DetectPizza();
        return TRUE;
    }
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
    DeliverySequence *best_seq;
    // The pizzas for the corresponding orders above
    Pizza *current_pizza, *next_pizza;
    unsigned char i = 0;
    // Get pizzas that I can pick up by the time I can get to them
    next_order = GetNextOrder(our_timeline, 0);
    next_pizza = GetPizzaForOrder(next_order);
    if (next_pizza->found == FALSE) {
        // printf("Next not found, going to normal");
        SetState('b');
        return;
    }
    available_pizzas = GetAvailablePizzas();
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
        // printf("Reset state: %c %c", current_pizza->colour, current_pizza->size);
    }

    for (i = 0; i < available_pizzas->len; i++) {
        // If the pizza's location isn't known, we don't want to waste time
        // with extra orders. We need to find this pizza!
        // TODO: Special case for canceled orders?
        current_pizza = available_pizzas->pizzas[i];
        if (next_pizza != NULL && next_pizza->location == NULL) {
            SetState('b');
            return;
        }
        current_order = GetOrderForPizza(current_pizza);

        // printf("Free time, considering combos");
        // Display(current_order);
        // printf("Debug %d: %d %d", next_extra_order == NULL, current_pizza == NULL, ConsiderCancel(next_order, current_order)->should_cancel);
        if (current_pizza != NULL) {
            // printf(" %d", current_pizza->location == NULL);
        }
        // printf("");

        best_seq = ConsiderCancel(next_order, current_order);
        if (
            current_pizza->found == TRUE &&
            current_pizza != NULL && current_pizza->location != NULL &&
            current_order != next_order &&

            best_seq->should_cancel == FALSE
        ) {
            next_extra_order = current_order;
            // printf("Extra: %s at %d, found %d", next_extra_order->delivery_house->name, next_extra_order->order_time, current_pizza->found);
            break;
        }
        free(best_seq->order_combo);
        free(best_seq->pizza_combo);
        free(best_seq);
    }
    free(available_pizzas->pizzas);
    free(available_pizzas);
    // If no pizzas were found that satisfied the conditions above, we should consider looking for
    // more pizzas
    if (next_extra_order == NULL) {
        // fs takes care of considering cost and everything.
        // If it thinks it'll delay us to find more pizzas, it'll change the state to 'b'
        // printf("Finding pizzas...");
        FindPizzas();
    }
    else {
        SetState('b');
    }
// // ////    printf("Free time over");
}


Node *GetDepForHouse(Node *house) {
    Node *dep_a, *dep_b;
    
    if (house->name[0] != 'H') {
        // It's not a house, so it has no deposition zone
        return NULL;
    }

    dep_a = GetNodeByName(strcat(house->name, "DA"));
    dep_b = GetNodeByName(strcat(house->name, "DB"));

    if (IsPizzaAt(dep_a, TRUE)) {
        if (IsPizzaAt(dep_b, TRUE)) {
            // printf("Too much pizza is bad for you");
            return NULL;
        }
        else {
            return dep_b;
        }
    }
    else {
        return dep_a;
    }
}

void DeliverPizzas(DeliverySequence *cur_sequence) {
    Pizza *delivered_pizza;
    Order *current_order;
    Node *cur_node, *deposition_zone;
    BotInfo *bot_info;
    float deg_to_dep = 0;
    float deg_to_pizza = -90;
    float current_arm_deg = 0;
    Arm *correct_arm;
    bot_info = GetBotInfo();

    // printf("Delivering");

    unsigned char pick1 = cur_sequence->pick1;
    unsigned char pick2 = cur_sequence->pick2;
    unsigned char deliver1 = cur_sequence->deliver1;
    unsigned char deliver2 = cur_sequence->deliver2;

    delivered_pizza = cur_sequence->pizza_combo[pick1];
    cur_node = delivered_pizza->location;
    if (cur_node != NULL) {
        // printf("Picking 1");
        MoveBotToNode(cur_node);
        delivered_pizza->state = 'd';
        // delivered_pizza->location = NULL;
        
        correct_arm = GetFreeArm();
        current_arm_deg = bot_info->cur_position->cur_deg + correct_arm->angle;
        deg_to_pizza = -90;
        RotateBot((int) GetShortestDeg(deg_to_pizza - current_arm_deg), FALSE);

        if (cur_sequence->order_combo[pick1]->pickup_time > GetCurrentTime()) {
            // TODO: Consider this as free time if possible?
            
            // printf("Early wait %d %d %d", cur_sequence->order_combo[pick1]->pickup_time - GetCurrentTime(), cur_sequence->order_combo[pick1]->pickup_time, GetCurrentTime());
            CustomDelay((cur_sequence->order_combo[pick1]->pickup_time - GetCurrentTime()) * 1000);
            // sleep((cur_sequence->order_combo[pick1]->pickup_time - GetCurrentTime()));
        }
        PickPizzaUp(delivered_pizza);
    }

    cur_node = NULL;
    delivered_pizza = cur_sequence->pizza_combo[pick2];
    if (delivered_pizza) {
        cur_node = delivered_pizza->location;
    }
    if (cur_node != NULL) {
        // printf("Picking 2");
        MoveBotToNode(cur_node);
        delivered_pizza->state = 'd';
        delivered_pizza->dep_loc = NULL;

        
        correct_arm = GetFreeArm();
        current_arm_deg = bot_info->cur_position->cur_deg + correct_arm->angle;
        deg_to_pizza = -90;
        RotateBot((int) GetShortestDeg(deg_to_pizza - current_arm_deg), FALSE);

        if (cur_sequence->order_combo[pick2]->pickup_time > GetCurrentTime()) {
            // TODO: Consider this as free time if possible?
            
            // printf("Early wait %d %d %d", cur_sequence->order_combo[pick2]->pickup_time - GetCurrentTime(), cur_sequence->order_combo[pick2]->pickup_time, GetCurrentTime());
            CustomDelay((cur_sequence->order_combo[pick2]->pickup_time - GetCurrentTime()) * 1000);
            // sleep((cur_sequence->order_combo[pick2]->pickup_time - GetCurrentTime()));
        }
        PickPizzaUp(delivered_pizza);
    }

    current_order = cur_sequence->order_combo[deliver1];
    cur_node = current_order->delivery_house;
    if (cur_node != NULL) {
        RgbLedGlow(current_order->colour);
        MoveBotToNode(cur_node);
        deposition_zone = GetDepForHouse(current_order->delivery_house);
        deg_to_dep = RadToDeg(atan2(deposition_zone->y - cur_node->y, deposition_zone->x - cur_node->x));
        if(bot_info->arm1->carrying == delivered_pizza) {
            correct_arm = bot_info->arm1;
        }
        else {
            correct_arm = bot_info->arm2;
        }
        // The current arm angle in absolute terms
        current_arm_deg = bot_info->cur_position->cur_deg + correct_arm->angle;

        RotateBot((int) GetShortestDeg(deg_to_dep - current_arm_deg), FALSE);

        if (GetCurrentTime() < current_order->delivery_period->start) {
            // printf("Early dwait %d", (int) (current_order->delivery_period->start - GetCurrentTime()));
            CustomDelay((current_order->delivery_period->start - GetCurrentTime()) * 1000);
            // sleep((current_order->delivery_period->start - GetCurrentTime()));
        }
        // printf(" by %d", GetCurrentTime());
        orders_completed++;
        delivered_pizza->dep_loc = deposition_zone;
        current_order->state = 'd';
        DepositPizza(delivered_pizza);
        RgbLedOff();
        BuzzerOn();
        FreezeDisplay();
        if (RegisterCallback(BuzzerOff, 1) == 0) {
            _delay_ms(1000);
            BuzzerOff();
        };
    }

    cur_node = NULL;
    current_order = cur_sequence->order_combo[deliver2];
    if (current_order) {
        cur_node = current_order->delivery_house;
    }
    if (cur_node != NULL) {
        RgbLedGlow(current_order->colour);
        MoveBotToNode(cur_node);
        deposition_zone = GetDepForHouse(current_order->delivery_house);
        deg_to_dep = RadToDeg(atan2(deposition_zone->y - cur_node->y, deposition_zone->x - cur_node->x));
        if(bot_info->arm1->carrying == delivered_pizza) {
            correct_arm = bot_info->arm1;
        }
        else {
            correct_arm = bot_info->arm2;
        }
        // The current arm angle in absolute terms
        current_arm_deg = bot_info->cur_position->cur_deg + correct_arm->angle;

        RotateBot((int) GetShortestDeg(deg_to_dep - current_arm_deg), FALSE);
        if (GetCurrentTime() < current_order->delivery_period->start) {
            // printf("early dwait %d", (int) (current_order->delivery_period->start - GetCurrentTime()));
            CustomDelay((current_order->delivery_period->start - GetCurrentTime()) * 1000);
            // sleep((current_order->delivery_period->start - GetCurrentTime()));
        }
        // printf(" by %d", GetCurrentTime());
        orders_completed++;
        delivered_pizza->dep_loc = deposition_zone;
        current_order->state = 'd';
        DepositPizza(delivered_pizza);
        RgbLedOff();
        BuzzerOn();
        FreezeDisplay();
        if (RegisterCallback(BuzzerOff, 1) == 0) {
            _delay_ms(1000);
            BuzzerOff();
        };
    }
    // printf("*** Delivered pizzas! ");
    if (cur_sequence->order_combo[deliver1] != NULL) {
        // printf("%s, %d", cur_sequence->order_combo[deliver1]->delivery_house->name, cur_sequence->order_combo[deliver1]->order_time);
    }
    if (cur_sequence->order_combo[deliver2] != NULL) {
        // printf("%s, %d ", cur_sequence->order_combo[deliver2]->delivery_house->name, cur_sequence->order_combo[deliver2]->order_time);
    }

    next_extra_order = NULL;
    SetState('f');
    free(cur_sequence->order_combo);
    free(cur_sequence->pizza_combo);
    free(cur_sequence);
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
    char time_to_find = TRUE;

    next_reg_order = GetNextOrder(our_timeline, 0);
    // If we've delivered all the regular orders already, we can consider the canceled pizzas
    // as our regular orders.
    if (next_reg_order == NULL && next_extra_order != NULL) {
        next_reg_order = next_extra_order;
        next_extra_order = NULL;
        // printf("Shuffling extra and reg around");
    }
    if (next_reg_order) {
        printf("Reg: %s, %d", next_reg_order->delivery_house->name, next_reg_order->order_time);
    }
    // Display(next_reg_order);
    if (next_extra_order) {
        printf("Ex: %s, %d", next_extra_order->delivery_house->name, next_extra_order->order_time);
    }
    // Display(next_extra_order);
    next_reg_pizza = GetPizzaForOrder(next_reg_order);
    if (next_reg_pizza == NULL) {
        // printf("NEXT_REG IS NULL!");
    }
    // TODO: Special case for canceled orders
    if (next_reg_pizza->found == FALSE) {
        if (total_pizzas == MAX_ORDERS) {
            // This means that the pizza we want is missing, because we've found as many
            // pizzas as there are orders (including fake pizzas) and the one we want
            // isn't in that list
            MissingOrderBeep();
            next_reg_pizza->state = 'd';
            next_reg_order->state = 'd';
            orders_completed++;
            SetState('f');
            return;
        }
        // If the location of our regular order is unknown, then we want to find it
        // But we want to cancel it if finding it will delay our future orders
        time_to_find = FindPizzas();
        if (time_to_find == FALSE) {
            // Finding the current pizza seems like it'll delay us,
            // so lets skip this order
            printf("Canc (404) %s at %d %c", next_reg_order->delivery_house->name, next_reg_order->order_time, next_reg_order->order_type);
            next_reg_order->state = 'c';
            // Now that we've canceled it, maybe we have extra time to find pizzas or pick up
            // extra pizzas, so we consider it free time
            SetState('f');
            return;
        }

        // If our call to FindPizzas did find it, we want to consider free time now, else,
        // we want to come right back here and consider searching more or canceling
        if (next_reg_pizza->found == TRUE) {
            SetState('f');
        }
        // We can't consider delivering the pizza if we haven't found it
        // If we have found it, we want to consider it free time first!
        return;
    }
    // next_extra_order might be NULL, but that's okay, because ConsiderCancel
    // can deal with it.
    cur_sequence = ConsiderCancel(next_reg_order, next_extra_order);
    // printf("Should cancel: %d cur_time: %d", cur_sequence->should_cancel, GetCurrentTime());

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
        // printf("Canceling %s at %d %c", next_reg_order->delivery_house->name, next_reg_order->order_time, next_reg_order->order_type);
        next_reg_order->state = 'c';
        free(cur_sequence->order_combo);
        free(cur_sequence->pizza_combo);
        free(cur_sequence);
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
        // printf("time: %d, o: %d, p: %d", GetCurrentTime(), orders_completed, total_pizzas);
        if (orders_completed == total_orders) {
            // printf("All done!");
            VictoryTime();
            BuzzerBeep(5000);
            return;
        }
        if (GetState() == 'f') {
            // printf("Free time!");
            FreeTimeDecision(); // this will set the state to busy if it didn't find any pizzas to deliver
        }
        else {
            // printf("Normal operation!");
            NormalOperation(); // this will set the state to free when the delivery is done
        }
    }
}

void Display(Order *current_order) {
    if (current_order == NULL) {
        return;
    }
    // printf("colour: %c, ", current_order->colour);
    // printf("size: %c, ", current_order->size);
    // printf("time: %d, ", current_order->order_time);
    // printf("ptime: %d, ", current_order->pickup_time);
    // printf("type: %c, ", current_order->order_type);
    // printf("state: %c, ", current_order->state);
    // printf("house: %s", current_order->delivery_house->name);
   // // //// printf("pickup point: %s", current_order->pickup_point);
}
