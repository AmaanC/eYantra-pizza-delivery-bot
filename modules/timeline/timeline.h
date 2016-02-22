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
    unsigned char found;
    // Location of the pizza. If the found flag is 0, the location is just a guess
    Node *location;
    Node *dep_loc;
} Pizza;

typedef struct _PizzaList {
    // An array of pizzas
    Pizza **pizzas;
    // We won't be making lists with a length of 255, so an char is okay
    unsigned char len;
} PizzaList;

typedef struct _Order {
    // r, g, b: red, green, blue
    char colour;
    // s, m, l: small, medium, large
    char size;
    // 30, for example
    // Max order time is 600, so an unsigned char won't do, but a short will
    short int order_time;
    // Preorders can be picked up at any time
    // Regular orders can only be picked up after their order time
    // This lets us easily differentiate
    short pickup_time;
    // r, p: regular, preorder
    char order_type;
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
    unsigned char len;
} OrderList;

typedef struct _DeliverySequence {
    // When there are 2 orders, this structure allows us to figure out what the sequence of actions should be
    unsigned char pick1; // If it's 1, we pick order 1 first, if it's 2 we pick order2 first
    unsigned char pick2; // TODO
    unsigned char deliver1;
    unsigned char deliver2;
    Order **order_combo;
    Pizza **pizza_combo;
    // 0 or 1 indicating whether this delivery sequence is one that shouldn't be used
    char should_cancel;
    float total_cost;
} DeliverySequence;

Node *GetNodeToRight(Node *source_node);
Node *GetNodeToLeft(Node *source_node);
Node *GetFirstPToRight(char real_pizza);
Node *GetFirstPToLeft(char real_pizza);
Node *GetFurthestPizzaNode(Node *source_node);
char GetState();
void SetState(char new_state);
Node *GetPizzaCounter();
void InsertOrder(OrderList *timeline, Order *new_order);
void InsertPizza(PizzaList *pizza_list, Pizza *new_pizza);
Pizza *CreatePizza(char colour, char size);
void InitTimeline();
OrderList *GetTimeline();
PizzaList *GetPizzas();
void MissingOrderBeep();
void FindNextDefiniteNeed(OrderList *timeline);
char GetNumDelayed(Node *source_node, char start_time, char order_num);
DeliverySequence *ConsiderCancel(Order *order1, Order *order2);
Order *GetNextOrder(OrderList *timeline, char pos);
Pizza *GetPizzaForOrder(Order *order);
Order *GetOrderForPizza(Pizza *pizza);
Pizza *GetPizzaAtNode(Node *node);
PizzaList *GetAvailablePizzas();
void DetectPizza();
char IsPizzaAt(Node *test_node, char real_pizza);
char FindPizzas();
TimeBlock *GetTimeReqForOrders(Order *order1, Order *order2);
void FreeTimeDecision();
void DeliverPizzas(DeliverySequence *cur_sequence);
void NormalOperation();
void TimelineControl();
void Display(Order *current_order);

#endif
