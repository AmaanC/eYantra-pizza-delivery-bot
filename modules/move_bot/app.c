#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> 
#include "../bl_sensor/bl_sensor.h"
#include "../pos_encoder/pos_encoder.h"
#include "move_bot.h"


int main(){
	move_bot_init_devices();
	move_bot_forward(100);
	_delay_ms(500);
	rotate_bot(90);
	_delay_ms(500);
}