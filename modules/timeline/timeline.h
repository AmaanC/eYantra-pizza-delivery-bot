#ifndef PIZZA_H_
#define PIZZA_H_

#include "../map/map.h"

typedef struct _Pizza {
    char colour; // r, g, b: red, green, blue
    char size; // s, m, l: small, medium, large
    int order_time; // 30, for example
    char order_type; // r, p: regular, preorder
    Node *delivery_house; // it is a node now, not just a name.
    Node *pickup_point; // this too.
    char status;
    //'f' = found
    //'n' = not found
    //'d' = delivered
    //'h' = holding
    //'c' = canceled
    Time_Block *block;
} Pizza;

Pizza *CreatePizza(char colour, char size, int order_time, char order_type, char *delivery_house_name);

typedef struct _Time_Block {
	int start_time;
	int end_time;
}Time_Block;

#endif
