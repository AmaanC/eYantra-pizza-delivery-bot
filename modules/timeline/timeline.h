#ifndef PIZZA_H_
#define PIZZA_H_

#include "../map/map.h"

typedef struct _Pizza {
    char colour; // r, g, b: red, green, blue
    char size; // s, m, l: small, medium, large
    int order_time; // 30, for example
    char order_type; // r, p: regular, preorder
    Node *delivery_house; // 
    Node *pickup_point;
} Pizza;

Pizza *CreatePizza(char colour, char size, int order_time, char order_type, char *delivery_house_name);

#endif