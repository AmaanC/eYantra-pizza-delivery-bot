#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "timer.h"

uintmax_t start_time = 0;

void TimerInit() {
    start_time = time(NULL);
}

uintmax_t GetTime() {
    return ((uintmax_t) time(NULL) - start_time);
}