#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void pos_encoder_left_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void pos_encoder_right_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

//Function to initialize ports
void pos_encoder_port_init()
{
 pos_encoder_left_pin_config(); //left encoder pin config
 pos_encoder_right_pin_config(); //right encoder pin config	
}

void pos_encoder_left_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt 
}

void pos_encoder_right_interrupt_init (void) //Interrupt 5 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
 EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
 sei();   // Enables the global interrupt 
}

//ISR for right position encoder
ISR(INT5_vect)  
{
 ShaftCountRight++;  //increment right shaft position count
}

//ISR for left position encoder
ISR(INT4_vect)
{
 ShaftCountLeft++;  //increment left shaft position count
}

//Function to initialize all the devices
void pos_encoder_init_devices()
{
 cli(); //Clears the global interrupt
 pos_encoder_port_init();  //Initializes all the ports
 pos_encoder_left_interrupt_init();
 pos_encoder_right_interrupt_init();
 sei();   // Enables the global interrupt 
}

// The following functions can be used by first storing
// the previous value, and then polling the value using
// this function to see how far the motor has traveled.
unsigned long int pos_encoder_get_left() {
  return ShaftCountLeft;
}

unsigned long int pos_encoder_get_right() {
  return ShaftCountRight;
}