#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"
#include "../map/map.h"
#include "../bot_memory/bot_memory.h"
#include "../timer/timer.h"
#include "../lcd/lcd.h"
#include "../move_bot/move_bot.h"

int main() {
    int i = 0;
    // sharp_init_devices();
    InitTimer();
    InitBotInfo();
    InitGraph();
    InitTimeline();
    LcdInit();
    MoveBotInitDevices();
    PizzaList *our_pizzas = GetPizzas();
    for (i = 0; i < our_pizzas->len; i++) {
        printf("C: %c, S: %c", our_pizzas->pizzas[i]->colour, our_pizzas->pizzas[i]->size);
    }

    // timeline = GetTimeline();
    // printf("Num orders: %d\n", timeline->len);
    // // printf("Order 2 start: %f\n", timeline->orders[2]->delivery_period->start);
    // FindNextDefiniteNeed(timeline);
    
    // for (i = 0; i < timeline->len; i++) {
    //     Display(timeline->orders[i]);
    // }
    // printf("Estimated cost of delivery 0: %f\n", EstimateNextCost(GetCurrentNode(), 0));
    // printf("Order 0 and 2 at the same time: %d\n", ConsiderCancel(timeline->orders[1], timeline->orders[2])->should_cancel);

    // So what happens initially is that we call our free time function, and since we've
    // found 0 pizzas, no pizzas will be considered right now.
    // Since we have time to kill until our first order, we find pizzas
    // Everytime we find a pizza, we consider picking it up using our
    // free time function

    // If we didn't have time to kill until our first order, we'd call our
    // normal delivery function, and that would note that we haven't found the
    // required pizza yet, so we'd call the find pizza function
    // When we find the pizza, we'd call the free time function, and that would
    // realize that we had no free time, so it would call the normal delivery
    // function, which would decide to pick the pizza up and deliver it right now

    // When the pizza has been delivered, we'd consider it free time and the
    // cycle repeats
    // In finding our first normal delivery pizza, we probably found a few others
    // and those pizzas might be available for pick up in this free time
    // Using the same algorithm, they may be eliminated
    // If they are, then we continue and deliver pizzas in order of their due times
    // If some pizzas remain (not eliminated), then we can pick them up without
    // any repurcussions

    // TODO: Think about our blocked_time, which is for times when we definitely need
    // 2 arms
    // What happens if we've found one of the pizzas, and not found the other?

    // for (i = 0; i < 4; i++) {
    //     order = GetNextOrder(GetTimeline());
    //     printf("%s\n", order->delivery_house->name);
    //     order->state = 'f';
    // }

    // printf("Available:\n\n");
    // available = GetAvailablePizzas(GetCurrentTimeBlock());
    // for (i = 0; i < available->len; i++) {
    //     Display(available->orders[i]);
    // }
    // printf("Hello world");
    while (1) {
        TimelineControl();
    }
    return 0;
}