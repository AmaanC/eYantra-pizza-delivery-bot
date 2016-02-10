#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pos_encoder.h"

unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

//Function to configure ports to enable robot's motion
void pos_encoder_motion_pin_config (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void left_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void right_encoder_pin_config (void)
{
 DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
 PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

//Function to initialize ports
void pos_encoder_port_init()
{
 pos_encoder_motion_pin_config(); //robot motion pins config
 left_encoder_pin_config(); //left encoder pin config
 right_encoder_pin_config(); //right encoder pin config	
}

void left_position_encoder_interrupt_init (void) //Interrupt 4 enable
{
 cli(); //Clears the global interrupt
 EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
 EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
 sei();   // Enables the global interrupt 
}

void right_position_encoder_interrupt_init (void) //Interrupt 5 enable
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


//Function used for setting motor's direction
void pos_encoder_motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 		// removing upper nibbel for the protection
 PortARestore = PORTA; 		// reading the PORTA original status
 PortARestore &= 0xF0; 		// making lower direction nibbel to 0
 PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
 PORTA = PortARestore; 		// executing the command
}

void pos_encoder_forward (void) //both wheels forward
{
  pos_encoder_motion_set(0x06);
}

void pos_encoder_back (void) //both wheels backward
{
  pos_encoder_motion_set(0x09);
}

void pos_encoder_left (void) //Left wheel backward, Right wheel forward
{
  pos_encoder_motion_set(0x05);
}

void pos_encoder_right (void) //Left wheel forward, Right wheel backward
{
  pos_encoder_motion_set(0x0A);
}

void pos_encoder_soft_left (void) //Left wheel stationary, Right wheel forward
{
  pos_encoder_motion_set(0x04);
}

void pos_encoder_soft_right (void) //Left wheel forward, Right wheel is stationary
{
  pos_encoder_motion_set(0x02);
}

void pos_encoder_soft_left_2 (void) //Left wheel backward, right wheel stationary
{
  pos_encoder_motion_set(0x01);
}

void pos_encoder_soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
  pos_encoder_motion_set(0x08);
}

void pos_encoder_stop (void)
{
  pos_encoder_motion_set(0x00);
}


//Function used for turning robot by specified degrees
void pos_encoder_angle_rotate(unsigned int Degrees)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = (float) Degrees/ 4.090; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned int) ReqdShaftCount;
 ShaftCountRight = 0; 
 ShaftCountLeft = 0; 

 while (1)
 {
  if((ShaftCountRight >= ReqdShaftCountInt) | (ShaftCountLeft >= ReqdShaftCountInt))
  break;
 }
  pos_encoder_stop(); //Stop robot
}

//Function used for moving robot forward by specified distance

void pos_encoder_linear_distance_mm(unsigned int DistanceInMM)
{
 float ReqdShaftCount = 0;
 unsigned long int ReqdShaftCountInt = 0;

 ReqdShaftCount = DistanceInMM / 5.338; // division by resolution to get shaft count
 ReqdShaftCountInt = (unsigned long int) ReqdShaftCount;
  
 ShaftCountRight = 0;
 while(1)
 {
  if(ShaftCountRight > ReqdShaftCountInt)
  {
  	break;
  }
 } 
  pos_encoder_stop(); //Stop robot
}

void pos_encoder_forward_mm(unsigned int DistanceInMM)
{
 pos_encoder_forward();
 pos_encoder_linear_distance_mm(DistanceInMM);
}

void pos_encoder_back_mm(unsigned int DistanceInMM)
{
 pos_encoder_back();
 pos_encoder_linear_distance_mm(DistanceInMM);
}

void pos_encoder_left_degrees(unsigned int Degrees) 
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 pos_encoder_left(); //Turn left
 pos_encoder_angle_rotate(Degrees);
}


void pos_encoder_right_degrees(unsigned int Degrees)
{
// 88 pulses for 360 degrees rotation 4.090 degrees per count
 pos_encoder_right(); //Turn right
 pos_encoder_angle_rotate(Degrees);
}


void pos_encoder_soft_left_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 pos_encoder_soft_left(); //Turn soft left
 Degrees=Degrees*2;
 pos_encoder_angle_rotate(Degrees);
}

void pos_encoder_soft_right_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 pos_encoder_soft_right();  //Turn soft right
 Degrees=Degrees*2;
 pos_encoder_angle_rotate(Degrees);
}

void pos_encoder_soft_left_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 pos_encoder_soft_left_2(); //Turn reverse soft left
 Degrees=Degrees*2;
 pos_encoder_angle_rotate(Degrees);
}

void pos_encoder_soft_right_2_degrees(unsigned int Degrees)
{
 // 176 pulses for 360 degrees rotation 2.045 degrees per count
 pos_encoder_soft_right_2();  //Turn reverse soft right
 Degrees=Degrees*2;
 pos_encoder_angle_rotate(Degrees);
}

//Function to initialize all the devices
void pos_encoder_init_devices()
{
 cli(); //Clears the global interrupt
 pos_encoder_port_init();  //Initializes all the ports
 left_position_encoder_interrupt_init();
 right_position_encoder_interrupt_init();
 sei();   // Enables the global interrupt 
}

void pos_encoder_rotate_bot(int Degrees)
{
    if(Degrees == 90)
    {
      pos_encoder_left_degrees(90);
      pos_encoder_stop();     
      _delay_ms(500); 
    }
    elseif(Degrees == -90)
    {
      pos_encoder_right_degrees(90);
      pos_encoder_stop();     
      _delay_ms(500); 
    }

}
