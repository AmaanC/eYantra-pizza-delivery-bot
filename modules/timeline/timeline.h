#ifndef PIZZA_H_
#define PIZZA_H_

#include "../map/map.h"

// A period of time. We use it to check for overlaps between different time blocks
typedef struct _TimeBlock {
    float start;
    float end;
} TimeBlock;

typedef struct _Order {
    char colour; // r, g, b: red, green, blue
    char size; // s, m, l: small, medium, large
    int order_time; // 30, for example
    char order_type; // r, p: regular, preorder
    Node *delivery_house; // it is a node now, not just a name.
    Node *pickup_point; // this too.
    char state;
    // 'f' = found
    // 'n' = not found
    // 'd' = delivered
    // 'h' = holding
    // 'c' = canceled
    TimeBlock *block;
} Order;

// A timeline is just an array of orders
typedef struct _OrderList {
    Order **orders;
    int len;
} OrderList;

void CreateOrder(OrderList *timeline, char colour, char size, int order_time, char order_type, char *delivery_house_name);
OrderList *GetTimeline();
void FindNextDefiniteNeed(OrderList *timeline);
void Display(Order *current_order);
Order *GetNextOrder(OrderList *timeline);
OrderList *GetAvailablePizzas(TimeBlock *current_period);
TimeBlock *GetCurrentTimeBlock();

#endif
