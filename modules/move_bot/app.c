#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> 
#include "../bl_sensor/bl_sensor.h"
#include "../pos_encoder/pos_encoder.h"
#include "move_bot.h"


int main() {
    MoveBotInitDevices();
    // LcdInit();
    while (1) {
        RotateBot(90);
        _delay_ms(1000);
        RotateBot(-90);
        _delay_ms(1000);
    }
    // PosEncoderVelocity(255, 255);
    // MoveBotForward(100);
    _delay_ms(500);
    // Consider a rotate_towards(Node *target) function instead?
    // That can deduce whether black lines exist or not too
    //RotateBot(90);
    _delay_ms(500);
    return 0;
}