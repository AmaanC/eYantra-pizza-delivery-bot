#ifndef PIZZA_H_
#define PIZZA_H_

typedef struct _Pizza {
    char colour; // r, g, b: red, green, blue
    char size; // s, m, l: small, medium, large
    int order_time; // 30, for example
    char order_type; // r, p: regular, preorder
    char *delivery_house; // 
    char *pickup_point;
} Pizza;

#endif