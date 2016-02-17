#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timeline.h"
#include "../map/map.h"
#include "../dijkstra/dijkstra.h"
#include "../bot_memory/bot_memory.h"

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

// Number of pizzas found to the left of the pizza counter and to
// the right of the pizza counter
// We can use these values to decide which side is likelier to yield more results
int left_pizzas = 0;
int right_pizzas = 0;

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
    pizza->state = 'f';
    pizza->location = NULL;

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
        new_order->delivery_period->start = order_time - GUARANTEED_PERIOD;
        new_order->delivery_period->end = order_time + GUARANTEED_PERIOD;
    }
    else {
        new_order->delivery_period->start = order_time;
        new_order->delivery_period->end = order_time + GUARANTEED_PERIOD;
    }

    InsertOrder(timeline, new_order);
    // printf("%d: %s\n", i, delivery_house_name);
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
    CreateOrder(our_timeline, 'r', 'l', 30, 'r', "H12");
    CreateOrder(our_timeline, 'g', 'l', 70, 'p', "H2");
    CreateOrder(our_timeline, 'r', 'l', 50, 'r', "H4");
    CreateOrder(our_timeline, 'b', 'l', 89, 'r', "H4");
    CreateOrder(our_timeline, 'b', 'l', 0, 'r', "H6");
    CreateOrder(our_timeline, 'r', 'l', 30, 'p', "H6");
}

OrderList *GetTimeline() {
    return our_timeline;
}

// TODO: Consider adding a threshold. An overlap of 1s means very little
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

TimeBlock *GetCurrentTimeBlock() {
    TimeBlock *current;
    current = malloc(sizeof(TimeBlock));
    // TODO: USE TIMER TO GET CURRENT TIME
    // current->start = TimerGetTime();
    current->start = 0;
    current->end = current->start + 1;
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
                printf("Overlap between %s, %f and %f\n", order1->delivery_house->name, order1->delivery_period->start, order2->delivery_period->start);
            }
            
        }
    }
    printf("Next period: %f to %f\n", next_potential->start, next_potential->end);
    // Out of all the required periods available, the one with the lowest time
    // is set to next_potential, so we set that to the next_required_period
    next_required_period = next_potential;
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
    Pizza *pizza_combo[] = {GetPizzaForOrder(order1), GetPizzaForOrder(order2)};
    Pizza *single_pizza;
    BotInfo *bot_info = GetBotInfo();
    Graph *our_graph = GetGraph();
    DeliverySequence *best_seq;
    best_seq = malloc(sizeof(DeliverySequence));
    best_seq->should_cancel = FALSE;

    float lowest_cost = INFINITY;
    float temp_cost = INFINITY;

    // If either order is null, then there's only one way to deliver the remaining order
    // Go pick it up, deliver it
    if (order1 == NULL || order2 == NULL) {
        if (order2 == order1) {
            printf("Both orders are NULL, doofus.\n");
            return NULL;
        }
        single_order = (order1 == NULL) ? order2 : order1;
        single_pizza = GetPizzaForOrder(single_order);
        temp_cost = Dijkstra(bot_info->cur_position->cur_node, single_pizza->location, bot_info->cur_position->cur_deg, our_graph)->total_cost;
        temp_cost += Dijkstra(single_pizza->location, single_order->delivery_house, single_pizza->location->enter_deg, our_graph)->total_cost;

        best_seq->pick1 = single_pizza->location;
        best_seq->deliver1 = single_order->delivery_house;
        if (temp_cost + EstimateNextCost(single_order->delivery_house, 1) > GetNextOrder(our_timeline, 1)->delivery_period->end) {
            best_seq->should_cancel = TRUE;
        }
        return best_seq;
    }




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

    for (i = 0; i < num_combos; i++) {
        for (j = 0; j < num_combos; j++) {
            if (i != j) {
                for (a = 0; a < num_combos; a++) {
                    for (b = 0; b < num_combos; b++) {
                        if (
                            a != b &&
                            pizza_combo[i] != NULL && pizza_combo[i]->location != NULL &&
                            pizza_combo[j] != NULL && pizza_combo[j]->location != NULL &&
                            order_combo[a]->delivery_house != NULL &&
                            order_combo[b]->delivery_house != NULL
                        ) {
                            printf("%d, %d - %d, %d\n", i, j, a, b);
                            // Get cost to pick i
                            temp_cost = Dijkstra(bot_info->cur_position->cur_node, pizza_combo[i]->location, bot_info->cur_position->cur_deg, our_graph)->total_cost;
                            // To pick j
                            temp_cost += Dijkstra(pizza_combo[i]->location, pizza_combo[j]->location, pizza_combo[i]->location->enter_deg, our_graph)->total_cost;
                            // To deliver a
                            temp_cost += Dijkstra(pizza_combo[j]->location, order_combo[a]->delivery_house, pizza_combo[j]->location->enter_deg, our_graph)->total_cost;
                            // To deliver b
                            temp_cost += Dijkstra(order_combo[a]->delivery_house, order_combo[b]->delivery_house, order_combo[a]->delivery_house->enter_deg, our_graph)->total_cost;

                            if (temp_cost < lowest_cost) {
                                lowest_cost = temp_cost;
                                best_seq->pick1 = pizza_combo[i]->location;
                                best_seq->pick2 = pizza_combo[j]->location;
                                best_seq->deliver1 = order_combo[a]->delivery_house;
                                best_seq->deliver1 = order_combo[b]->delivery_house;
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
    for (i = 0; i < timeline->len; i++) {
        current_order = timeline->orders[i];
        // If it hasn't already been canceled, delivered, or picked up, this is our next order
        if (current_order->state != 'c' && current_order->state != 'd' && current_order->state != 'h') {
            // printf("%c\n", current_order->state);
            next_reg_order = current_order;
            if (num_found == pos) {
                break;
            }
            num_found++;
        }
    }
    return next_reg_order;
}

Pizza *GetPizzaForOrder(Order *order) {
    int i = 0;
    Pizza *current_pizza;
    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        if (
            current_pizza->state != 'c' &&
            current_pizza->size == order->size &&
            current_pizza->colour == order->colour
        ) {
            return current_pizza;
        }
    }
    return NULL;
}

// Estimate the cost to delivering the next regular order when starting from the source_node
float EstimateNextCost(Node *source_node, int pos) {
    Order *next_order;
    Pizza *matching_pizza;
    // If the pizza has been found, we'll set this to the pizza's location, otherwise
    // it'll probably be the least favourable place the pizza could be in
    // TODO: Think about this and actually implement it
    Node *pizza_location;
    Graph *our_graph;
    // Cost to finding the pizza / picking the pizza up
    float cost_to_pizza = 0;
    // Cost to actually delivering the pizza
    float cost_to_delivery = 0;

    our_graph = GetGraph();

    next_order = GetNextOrder(our_timeline, pos);
    matching_pizza = GetPizzaForOrder(next_order);
    pizza_location = matching_pizza->location;
    if (pizza_location == NULL) {
        // We don't know where the pizza is, so we have to estimate the time to find it
        // TODO: Actually implement this
        pizza_location = PIZZA_COUNTER_NODE;
        printf("Assuming pizza is at %s\n", PIZZA_COUNTER_NODE->name);
    }
    cost_to_pizza = Dijkstra(source_node, pizza_location, source_node->enter_deg, our_graph)->total_cost;
    cost_to_delivery = Dijkstra(pizza_location, next_order->delivery_house, pizza_location->enter_deg, our_graph)->total_cost;

    return cost_to_pizza + cost_to_delivery;
}

// Get all the pizzas that I can pick up in the current_period
// These are pizzas whose start times are past the start time of the current_period
// Example:
// If the bot is at H12, and has free time, it'll create a TimeBlock which
// starts at the current time + the time it'll need to go to the pizza counter
// This function will check which pizzas are available to be picked up in that time
// Whichever *are* available are returned in an OrderList
OrderList *GetAvailablePizzas(TimeBlock *current_period) {
    OrderList *available_pizzas;
    Order *current_order;
    int i = 0;
    available_pizzas = malloc(sizeof(OrderList));
    available_pizzas->orders = malloc(MAX_ORDERS * sizeof(Order));

    for (i = 0; i < our_timeline->len; i++) {
        current_order = our_timeline->orders[i];
        // If the current order has been found and it's delivery period overlaps with the current_period
        // it's an available pizza!
        if (CheckOverlap(current_period, current_order->delivery_period)) {
            InsertOrder(available_pizzas, current_order);
        }
    }
    return available_pizzas;
}

// This is called when we're already next to a pizza, and we simply want
// to detect the pizza and save its details in our Order struct
// The state will change to 'f' to indicate that its been found
void DetectPizza() {
    int i = 0;
    Pizza *current_pizza;
    BotInfo *bot_info;
    int found = FALSE;
    char block_size = 's'; // SharpGetBlockSize();
    char colour = 'r'; // ColourGet();
    
    // Check the bot's current position
    // If we already have a pizza detected at this location, we're detecting nothing new
    // If we haven't, then correlate it to our_pizzas, and attach the current location to it
    // Also, use our current location to see which side the pizza was on
    // That information will be used in the future to decide which side is likelier to have pizzas
    // Now that we've found a new pizza, set the state to free, and let FreeTime
    // decide if we want to pick it up or not

    // TODO: Consider bad readings and rechecking?
    bot_info = GetBotInfo();
    if (bot_info->cur_position->cur_node->x < PIZZA_COUNTER_NODE->x) {
        left_pizzas++;
    }
    else {
        right_pizzas++;
    }
    // There is no pizza block at this location
    if (block_size == 'n') {
        // In this situation, we want to CreatePizza with details that no order will have
        // so that we know that we've already checked this location, but we also
        // never try to pick anything up from this location
        CreatePizza('u', 'u');
        return;
    }

    for (i = 0; i < our_pizzas->len; i++) {
        current_pizza = our_pizzas->pizzas[i];
        if (
            current_pizza->colour == colour &&
            current_pizza->size == block_size &&
            current_pizza->location == NULL
        ) {
            found = TRUE;
            current_pizza->location = bot_info->cur_position->cur_node;
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
    }
}

int IsPizzaAt(Node *test_node) {
    int i = 0;
    int found = FALSE;
    for (i = 0; i < our_pizzas->len; i++) {
        if (our_pizzas->pizzas[i]->location == test_node) {
            found = TRUE;
        }
    }
    return found;
}

// Returns a node which is to the right (where x co-ordinate is higher) of the source_node
// It only looks through the source_nodes connections, not through all the nodes on the map
Node *GetNodeToRight(Node *source_node) {
    int i;
    Node *counter_node;
    for (i = 0; i < source_node->counter; i++) {
        counter_node = source_node->connected[i]->ptr;
        if (counter_node->x > source_node->x) {
            return counter_node;
        }
    }
}

// Consider looking for more pizzas
// This function will consider the cost of finding pizzas
// If the cost doesn't delay us for our regular order, it'll actually go search for pizzas
// Upon finding its first unknown pizza, it'll set the state to free (from within DetectPizza)
// If the cost does delay us, it'll set the state to 'b' so that NormalOperation can continue
void FindPizzas() {
    // We need to consider the cost first, and check our memory to see how many
    // pizzas we've found on each side
    BotInfo *bot_info;
    Graph *our_graph;
    PathStack *path_to_counter, *path_to_pizza;
    Node *target_pizza_node;
    // TODO: Discuss threshold value
    // The threshold is here because if we have only 1 second of grace time to find a pizza
    // and then get to the next regular order, I think we shouldn't risk it
    // So we need an appropriate threshold for that.
    const int threshold = 5;
    float cost = 0;
    bot_info = GetBotInfo();
    our_graph = GetGraph();
    
    if (left_pizzas + right_pizzas == MAX_ORDERS) {
        // No more pizzas left to find, let's just get to delivering them
        SetState('b');
        return;
    }

    // Get the path from the bot's current node to the pizza counter
    path_to_counter = Dijkstra(bot_info->cur_position->cur_node, PIZZA_COUNTER_NODE, bot_info->cur_position->cur_deg, our_graph);
    
    // If we've found more on the left of r1 than we have on the right
    // let's go to the right
    // Get the cost of going to the first pizza to the right, which hasn't been detected
    if (left_pizzas > right_pizzas) {
        // We go right
        target_pizza_node = GetNodeToRight(PIZZA_COUNTER_NODE);
        // If that one is already a known location in our list of pizzas, find the one to the right of that
        // Basically, find the first one to the right which is unknown
        while (IsPizzaAt(target_pizza_node) == TRUE) {
            if (target_pizza_node == NULL) {
                printf("No node to the right!\n");
                return;
            }
            target_pizza_node = GetNodeToRight(target_pizza_node);
        }
        printf("First unknown one to the right is: %s\n", target_pizza_node->name);
        path_to_pizza = Dijkstra(PIZZA_COUNTER_NODE, target_pizza_node, PIZZA_COUNTER_NODE->enter_deg, our_graph);
    }
    else {

    }

    cost = path_to_counter->total_cost;
    cost += path_to_pizza->total_cost;
    // We don't use Dijkstra's here because we may not have found the next regular pizza yet
    // and may have to estimate the time to finding it
    cost += EstimateNextCost(target_pizza_node, 0);

    // Now we check if current time + cost + next_order_cost >= next_order.end_time
    if (cost + threshold > GetNextOrder(our_timeline, 0)->delivery_period->end) {
        SetState('b');
        return;
    }
    else {
        // TODO:
        // MoveBotToNode(target_pizza_node);
        // DetectPizza();
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
    OrderList *available_pizzas;
    Order *current_order, *next_order;
    // The pizzas for the corresponding orders above
    Pizza *current_pizza, *next_pizza;
    int i = 0;
    available_pizzas = GetAvailablePizzas(GetCurrentTimeBlock());
    next_order = GetNextOrder(our_timeline, 0);
    next_pizza = GetPizzaForOrder(next_order);
    // Next pizza is being considered, so if another order tries to get this pizza,
    // we want it to skip over this pizza
    next_pizza->state = 'c';
    // Every OrderList is sorted by due time already, so the first one we find that
    // ConsiderCancel thinks won't delay orders is the one we consider delivering
    for (i = 0; i < available_pizzas->len; i++) {
        // If there is a pizza available for this order
        // and the order isn't our next_reg_order anyway
        // and doesn't overlap with our blocked time
        // and doesn't delay next orders
        // LET'S DO THIS!
        current_order = available_pizzas->orders[i];
        current_pizza = GetPizzaForOrder(current_order);
        if (
            current_pizza != NULL && current_pizza->location != NULL &&
            current_order == next_order &&
            !CheckOverlap(next_required_period, GetTimeReqForOrders(next_order, current_order)) &&
            ConsiderCancel(next_order, current_order)->should_cancel == FALSE
        ) {
            next_extra_order = current_order;
            break;
        }
    }
    // We want the pizza to be available to find for when we actually pick it up
    next_pizza->state = 'f';

    // If no pizzas were found that satisfied the conditions above, we should consider looking for
    // more pizzas
    if (next_extra_order == NULL) {
        // FindPizzas takes care of considering cost and everything.
        // If it thinks it'll delay us to find more pizzas, it'll change the state to 'b'
        FindPizzas();
    }

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
    Order *next_reg_order;
    next_reg_order = GetNextOrder(our_timeline, 0);
    // If there is a next_extra_order, we've already considered the cost of delivering these
    // 2 pizzas, and it won't delay us
    // So we can just go ahead and pick them up
    if (next_extra_order != NULL) {
        // PickPizzas(next_reg_order, next_extra_order);
    }
}

void TimelineControl() {
    SetState('f');
    while (1) {
        // both functions below will be blocking
        // this means that they will not return until they've completed their task
        // free will return only when it's picked up the extra pizza or called find_pizzas
        // or it has set the state to busy
        // reg will only return when it has delivered the pizzas the bot is carrying
        if (GetState() == 'f') {
            FreeTimeDecision(); // this will set the state to busy if it didn't find any pizzas to deliver
        }
        else {
            NormalOperation(); // this will set the state to free when the delivery is done
        }
    }
}

void Display(Order *current_order) {
    printf("colour: %c, ", current_order->colour);
    printf("size: %c, ", current_order->size);
    printf("order time: %d, ", current_order->order_time);
    printf("order type: %c, ", current_order->order_type);
    printf("house: %s\n", current_order->delivery_house->name);
    // printf("pickup point: %s\n", current_order->pickup_point);
}
