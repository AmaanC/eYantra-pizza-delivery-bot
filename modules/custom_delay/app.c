#include "custom_delay.h"
#include "../buzzer/buzzer.h"

int main() {
    while (1) {
        buzzer_on();
        custom_delay(100);
        buzzer_off();
        custom_delay(300);
    }
}