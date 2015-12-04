#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void seven_segment_pin_config(void)
{
    // Port D upper nibble for CA connections
    // Port J for a,b,c,d, e,f,g,DEC as per manual
    // Both things above are in decreasing order
    // i.e.
    // Port J = Pin 7, 6, 5...0

    DDRD = DDRD | 0xF0; // set upper nibble of port D to output
    DDRJ = DDRJ | 0xFF; // set all bits to output
}

void port_init(void)
{
    seven_segment_pin_config();
}

void init_devices(void)
{
    cli(); //Clears the global interrupt
    port_init();  //Initializes all the ports
    sei();   // Enables the global interrupt
}

int main(void)
{
    init_devices();
    PORTD = PORTD | 0x10;
    while(1)
    {
       PORTJ = 0x1F;
       _delay_ms(500);
       PORTJ = 0x01;
       _delay_ms(500);
   }
}
