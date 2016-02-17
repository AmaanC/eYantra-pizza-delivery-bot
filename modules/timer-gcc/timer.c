#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

uintmax_t start_time = 0;

void TimerInit() {
    start_time = time(NULL);
}

uintmax_t GetTime() {
    return ((uintmax_t) time(NULL) - start_time);
}

int main(void) {
    TimerInit();
    sleep(5);
    printf("%jd seconds passed\n", GetTime());

    return 0;
}