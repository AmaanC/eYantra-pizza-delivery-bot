#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

// Types:
// 0 is regular black node (unnamed on the flex map)
// 1 is a pizza pick up node
// 2 is a house door node
// 3 is a house deposit node
typedef struct node {
    unsigned int type;
    // Co-ordinates are relative to the "start" block
    int x;
    int y;
    struct node **connected;
} node_t;

// Now we create the map
// The journey is long and treacherous
// For we must brave many obstacles
// Some of which be tall
// Some be small
// Most be crises about poetic freedom and grammar instincts
// We shall prevail
// Defeat them against all odds

int main() {
    node_t Start;
    Start.x = 0;
    Start.y = 0;
    Start.connected = malloc(1 * sizeof(node_t));

    node_t R1;
    Start.connected[0] = &R1;
    return 0;
}