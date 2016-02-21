#ifndef PIZZA_H_
#define PIZZA_H_

#include "../map/map.h"

// A period of time. We use it to check for overlaps between different time blocks
typedef struct _TimeBlock {
    float start;
    float end;
} TimeBlock;

typedef struct _Pizza {
    // r, g, b: red, green, blue
    // Could also be 'n' for no pizza
    // Unknown is only used in situations where there is no pizza where we expected
    // so we're making a fake, unknown pizza, which will basically be ignored
    char colour;
    // s, m, l: small, medium, large
    // Could also be 'n' for no pizza
    char size;
    // State of the pizza
    // c: considered
    // f: free
    // d: delivered
    // The state is required for when 2 orders order the same pizza
    // If we consider order1, with pizza1, we need to make sure order2
    // has pizza2 available too. Without this state, GetPizzaForOrder
    // would have no way of knowing that pizza1 was for order1, and could
    // not be used for order2 as well, even though its colour and size match
    char state;
    // Flag to determine whether we've found the pizza or not
    int found;
    // Location of the pizza. If the found flag is 0, the location is just a guess
    Node *location;
    Node *dep_loc;
} Pizza;

typedef struct _PizzaList {
    // An array of pizzas
    Pizza **pizzas;
    int len;
} PizzaList;

typedef struct _Order {
    // r, g, b: red, green, blue
    char colour;
    // s, m, l: small, medium, large
    char size;
    // 30, for example
    int order_time;
    // Preorders can be picked up at any time
    // Regular orders can only be picked up after their order time
    // This lets us easily differentiate
    int pickup_time;
    // r, p: regular, preorder
    char order_type;
    Pizza *pizza;
    // Node containing the delivery location
    Node *delivery_house;
    // State of the order:
    // n: new
    // d: delivered
    // h: holding
    // c: canceled
    char state;
    // The period within which the order must be delivered.
    TimeBlock *delivery_period;
} Order;

// A timeline is just an array of orders
typedef struct _OrderList {
    Order **orders;
    int len;
} OrderList;

typedef struct _DeliverySequence {
    // When there are 2 orders, this structure allows us to figure out what the sequence of actions should be
    int pick1; // If it's 1, we pick order 1 first, if it's 2 we pick order2 first
    int pick2; // TODO
    int deliver1;
    int deliver2;
    Order **order_combo;
    Pizza **pizza_combo;
    // 0 or 1 indicating whether this delivery sequence is one that shouldn't be used
    int should_cancel;
    float total_cost;
} DeliverySequence;

void CreateOrder(OrderList *timeline, char colour, char size, int order_time, char order_type, char *delivery_house_name);
OrderList *GetTimeline();
void FindNextDefiniteNeed(OrderList *timeline);
void Display(Order *current_order);
Order *GetNextOrder(OrderList *timeline, int pos);
PizzaList *GetAvailablePizzas();
TimeBlock *GetCurrentTimeBlock();
Pizza *GetPizzaForOrder(Order *order);
float EstimateNextCost(Node *source_node, int pos);
DeliverySequence *ConsiderCancel(Order *order1, Order *order2);
PizzaList *GetPizzas();
Node *GetPizzaCounter();
Node *GetFurthestPizzaNode(Node *source_node);
Node *GetNodeToRight(Node *source_node);
Node *GetNodeToLeft(Node *source_node);

Node *GetFirstPToRight(int real_pizza);
Node *GetFirstPToLeft(int real_pizza);

int GetNumDelayed(Node *source_node, int start_time, int order_num);
int IsPizzaAt(Node *test_node, int real_pizza);

#endif
