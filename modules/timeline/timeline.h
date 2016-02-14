#ifndef PIZZA_H_
#define PIZZA_H_

typedef struct _Pizza {
    char colour;
    char size;
    int order_time;
    char order_type;
    char *delivery_house;
    char *pickup_point;
} Pizza;

#endif