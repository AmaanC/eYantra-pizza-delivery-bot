#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pos_encoder.h"


//Main Function

int main(void)
{
    pos_encoder_init_devices();

    while(1)
    {
        pos_encoder_forward_mm(100); //Moves robot forward 100mm
        pos_encoder_stop();
        _delay_ms(500);         
        
        pos_encoder_back_mm(100);   //Moves robot backward 100mm
        pos_encoder_stop();         
        _delay_ms(500);
        
        pos_encoder_left_degrees(90); //Rotate robot left by 90 degrees
        pos_encoder_stop();
        _delay_ms(500);
        
        pos_encoder_right_degrees(90); //Rotate robot right by 90 degrees
        pos_encoder_stop();
        _delay_ms(500);
        
        pos_encoder_soft_left_degrees(90); //Rotate (soft turn) by 90 degrees
        pos_encoder_stop();
        _delay_ms(500);
        
        pos_encoder_soft_right_degrees(90); //Rotate (soft turn) by 90 degrees
        pos_encoder_stop();
        _delay_ms(500);

        pos_encoder_soft_left_2_degrees(90); //Rotate (soft turn) by 90 degrees
        pos_encoder_stop();
        _delay_ms(500);
        
        pos_encoder_soft_right_2_degrees(90);   //Rotate (soft turn) by 90 degrees
        pos_encoder_stop();
        _delay_ms(500);
    }
}
