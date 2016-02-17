#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "timer.h"

int main(void) {
    InitTimer();
    sleep(1.2);
    printf("%jd seconds passed\n", GetCurrentTime());

    return 0;
}