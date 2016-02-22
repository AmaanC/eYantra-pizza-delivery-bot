#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dijkstra.h"
#include "../bot_memory/bot_memory.h"
#include "../map/map.h"

int main() {
    PathStack *final_path;
    int i;
    InitBotInfo();
    InitGraph();

    // LcdInit();
    // printf("Num nodes: %d\n", GetGraph()->num_nodes);
    // printf("S == current: %d\n", GetCurrentNode() == GetNodeByName("S"));
    // printf("Targ name: %s\n", GetNodeByName("r1")->name);

    final_path = Dijkstra(GetNodeByName("S"), GetNodeByName("H12"), 90.00, GetGraph());

    for (i = final_path->top - 1; i >= 0; i--) {
        printf("%s, ", final_path->path[i]->name);
    }

    printf("Total: %d\n", (int) final_path->total_cost);
    DijkstraFree(final_path);
    return 0;
}