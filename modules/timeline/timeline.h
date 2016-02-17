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
    // Could also be 'u' for unknown
    // Unknown is only used in situations where there is no pizza where we expected
    // so we're making a fake, unknown pizza, which will basically be ignored
    char colour;
    // s, m, l: small, medium, large
    // Could also be 'u' for unknown
    char size;
    // State of the pizza
    // c: considered
    // f: free
    // The state is required for when 2 orders order the same pizza
    // If we consider order1, with pizza1, we need to make sure order2
    // has pizza2 available too. Without this state, GetPizzaForOrder
    // would have no way of knowing that pizza1 was for order1, and could
    // not be used for order2 as well, even though its colour and size match
    char state;
    // Location of the pizza. If it hasn't been found, it'll be NULL
    Node *location;
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
    // r, p: regular, preorder
    char order_type;
    Pizza *pizza;
    // Node containing the delivery location
    Node *delivery_house;
    // State of the order:
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
    Node *pick1;
    Node *pick2;
    Node *deliver1;
    Node *deliver2;
    // 0 or 1 indicating whether this delivery sequence is one that shouldn't be used
    int should_cancel;
} DeliverySequence;

void CreateOrder(OrderList *timeline, char colour, char size, int order_time, char order_type, char *delivery_house_name);
OrderList *GetTimeline();
void FindNextDefiniteNeed(OrderList *timeline);
void Display(Order *current_order);
Order *GetNextOrder(OrderList *timeline, int pos);
OrderList *GetAvailablePizzas(TimeBlock *current_period);
TimeBlock *GetCurrentTimeBlock();
Pizza *GetPizzaForOrder(Order *order);

#endif
