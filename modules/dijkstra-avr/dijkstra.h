#ifndef DIJKSTRA_H_
#define DIJKSTRA_H_

#include "../map/map.h"

// A "stack" of Nodes. Len indicates the next empty element, so to use it as a stack, use
// for (i = top-1; i >= 0; i--)
typedef struct _PathStack {
    Node **path;
    int top;
    float total_cost;
} PathStack;

int InitNodesDijkstra(Node* current_node);
PathStack* Dijkstra(Node *source_node, Node *target_node, float cur_deg, Graph *our_graph);

#endif