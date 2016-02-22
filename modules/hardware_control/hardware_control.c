// #include <avr/io.h>
// #include <avr/interrupt.h>
// #include <util/delay.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "../map/map.h"
#include "../dijkstra/dijkstra.h"
#include "../pos_encoder/pos_encoder.h"
#include "../bot_memory/bot_memory.h"
#include "../lcd/lcd.h"
#include "../custom_delay/custom_delay.h"
#include "../hardware_control/hardware_control.h"


void CurveTowards(Node *source_node, Node *target_node) {
    // We can use the center of the circle and a curve node to find the radius
    // Using the radius, we can calculate the angular velocities required by
    // each of the wheels
    unsigned char fast_value = 0;
    unsigned char slow_value = 0;
    unsigned char left_motor, right_motor;
    unsigned char max_speed = 0xFF; // The max speed either motor can turn at
    const float wheel_dist = 7.5; // According to measurement
    float ratio = 1;
    float angular_velocity = 0;
    CurveInfo *curve_info = GetCurveInfo();

    float radius = 0; // The radius will be calculated dynamically
    // Needed to find the radius
    float xDist = source_node->x - curve_info->curve_center->x;
    float yDist = source_node->y - curve_info->curve_center->y;
    radius = sqrt(xDist * xDist + yDist * yDist);
    // printf("%f, %f\n", source_node->x, yDist);

    angular_velocity = GetAngularVelocity(source_node, target_node);
    // Linear velocity = radius * angular velocity
    // Our motors have different radiuses, which is why they have different linear velocities
    slow_value = (radius - wheel_dist) * fabs(angular_velocity);
    fast_value = (radius + wheel_dist) * fabs(angular_velocity);
    ratio = max_speed / fast_value;

    fast_value = max_speed;
    slow_value *= ratio;
    
    // if (GetCurveDirection(source_node, target_node) == 1) {
    if (angular_velocity > 0) {
        right_motor = fast_value;
        left_motor = slow_value;
    }
    else {
        right_motor = slow_value;
        left_motor = fast_value;
    }
    // printf("\tCurve %s to %s: %d, %d\n", source_node->name, target_node->name, left_motor, right_motor);
    // Start the motors on the path for the curve
    // pos_encoder_velocity(left_motor, right_motor);
    // Let them keep going until one of the motors has spun enough
    // pos_encoder_forward();
    // pos_encoder_angle_rotate(abs(angle * 180 / M_PI));
}

void MoveBotToNode(Node *target_node) {
    PathStack *final_path;
    Node *current_node, *next_node;
    int i;
    float xDist, yDist;
    BotInfo *bot_info = GetBotInfo();
    CurveInfo *curve_info = GetCurveInfo();
    Graph *our_graph = GetGraph();
    // printf("Moving\n");
    final_path = Dijkstra(GetCurrentNode(), target_node, bot_info->cur_position->cur_deg, our_graph);
    for (i = final_path->top - 1; i >= 0; i--) {
        // lcd_printf("%s", final_path->path[i]->name);
        // _delay_ms(500);
        printf("%s, ", final_path->path[i]->name);
    }
    // lcd_printf("Cost: %d", (int) final_path->total_cost);
    // _delay_ms(500);

    printf("\nTotal cost: %f\n", final_path->total_cost);
    // usleep(final_path->total_cost * 100 * 1000);
    sleep(final_path->total_cost);
    // CustomDelay(final_path->total_cost);

    // Now that we know the path to take, here's how we actually get there
    // To go from A to D
    // cur_pos = A
    // next_dest = B
    // If curve, use custom function, else
    // Rotate towards next_dest (skip if rotation diff < threshold like 5 degs)
    // Move forward dist using pos encoders
    // Repeat for next pair of nodes
    i = final_path->top - 1;
    current_node = bot_info->cur_position->cur_node;
    while (current_node != target_node) {
        i--;
        next_node = final_path->path[i];
        // If both the current and next nodes are part of our "curve_nodes", use
        // our curve function
        if (
            IndexOfNode(curve_info->curve_nodes, curve_info->curve_nodes_len, current_node) != -1 &&
            IndexOfNode(curve_info->curve_nodes, curve_info->curve_nodes_len, next_node) != -1
        ) {
            // lcd_printf("Curve");

            CurveTowards(current_node, next_node);
        }
        else {

            xDist = current_node->x - next_node->x;
            yDist = current_node->y - next_node->y;
            // lcd_printf("Rot: %d", (int) ((next_node->enter_deg - bot_info->cur_position->cur_deg)));
            // _delay_ms(500);
            // pos_encoder_rotate_bot((next_node->enter_deg - bot_info->cur_position->cur_deg));
            bot_info->cur_position->cur_deg = next_node->enter_deg;
            // pos_encoder_forward_mm(10 * sqrt(xDist * xDist + yDist * yDist));
        }

        bot_info->cur_position->cur_node = current_node;
        current_node = next_node;
    }
    bot_info->cur_position->cur_node = target_node;
    printf("Reached %s node.\n", target_node->name);
    free(final_path->path);
    free(final_path);
}