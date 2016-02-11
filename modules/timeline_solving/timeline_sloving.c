#include<stdio.h>

typedef struct pizza
{
	char colour;
	char size;
	int order_time;
	char order_type;
	char *delivery_house;
	char *pickup_point;
}pizza;

pizza* insert_values(char colour,
	char size,
	int order_time,
	char order_type,
	char *delivery_house,
	char *pickup_point)
{
	pizza* temp;
	temp = malloc(sizeof(pizza));
	temp->colour = colour;
	temp->size = size;
	temp->order_time = order_time;
	temp->order_type = order_type;
	temp->delivery_house = delivery_house;
	temp->pickup_point = pickup_point;
	return temp;
}

void display(pizza* temp)
{
	printf("colour: %c\n", temp->colour);
	printf("size: %c\n", temp->size);
	printf("order time: %d\n", temp->order_time);
	printf("order type: %c\n", temp->order_type);
	printf("house: %s\n", temp->delivery_house);
	printf("pickup point: %s\n", temp->pickup_point );
}

int main()
{
	pizza* temp;
	temp = malloc(sizeof(pizza));
	temp = insert_values('r','l',150,'R',"H2","P4");
	display(temp);
	return 0;
}