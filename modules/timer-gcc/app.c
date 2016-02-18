#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "timer.h"

int main(void) {
    int time = 10;
    InitTimer();
    sleep(1.2);
    printf("%d seconds passed\n", time - GetCurrentTime());

    return 0;
}