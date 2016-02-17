#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#include "timer.h"

uintmax_t start_time = 0;

void InitTimer() {
    start_time = time(NULL);
}

uintmax_t GetCurrentTime() {
    return ((uintmax_t) time(NULL) - start_time);
}