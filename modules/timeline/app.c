#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"
#include "../map/map.h"

int main() {
    Order *temp;
    InitGraph();
    temp = CreateOrder('r','l',150,'R',"H2");
    display(temp);
    return 0;
}