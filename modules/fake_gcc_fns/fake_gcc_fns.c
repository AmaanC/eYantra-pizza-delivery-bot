#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "fake_gcc_fns.h"
#include "../lcd/lcd.h"

#include <stdarg.h>
#include <stdio.h>

extern int printf(const char *fmt, ...) {
    //return 0;
    va_list argp;
    const int LCDLEN = 16;
    char str[LCDLEN];
    va_start(argp, fmt);

    vsnprintf(str, LCDLEN, fmt, argp);

    va_end(argp);

    LcdString(str);
    _delay_ms(1000);
    return 0;
}

void sleep(int ms) {

}