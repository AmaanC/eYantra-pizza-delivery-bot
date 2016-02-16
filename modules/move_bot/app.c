#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> 
#include "../bl_sensor/bl_sensor.h"
#include "../pos_encoder/pos_encoder.h"
#include "move_bot.h"


int main(){
	move_bot_init_devices();
	pos_encoder_velocity(255, 255);
	move_bot_forward_mm(100);
	_delay_ms(500);
    // Consider a rotate_towards(Node *target) function instead?
    // That can deduce whether black lines exist or not too
	rotate_bot(90);
	_delay_ms(500);
}