#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "timer.h"

int main(void) {
    int time = 10;
    InitTimer();
    usleep(1000 * 2000);
    printf("%d seconds passed\n", GetCurrentTime());

    return 0;
}