#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"
#include "../map/map.h"

int main() {
    Pizza *temp;
    InitGraph();
    temp = CreatePizza('r','l',150,'R',"H2");
    display(temp);
    return 0;
}