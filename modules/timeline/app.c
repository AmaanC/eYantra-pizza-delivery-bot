#include <stdio.h>
#include <stdlib.h>
#include "timeline.h"

int main() {
    Pizza* temp;
    temp = CreatePizza('r','l',150,'R',"H2","P4");
    display(temp);
    return 0;
}