#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "timer.h"

int main(void) {
    TimerInit();
    sleep(1.2);
    printf("%jd seconds passed\n", GetTime());

    return 0;
}