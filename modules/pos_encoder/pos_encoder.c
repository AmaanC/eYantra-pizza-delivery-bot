#include <math.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pos_encoder.h"
#include "../lcd/lcd.h"

volatile unsigned long int ShaftCountLeft = 0; //to keep track of left position encoder 
volatile unsigned long int ShaftCountRight = 0; //to keep track of right position encoder
unsigned int Degrees; //to accept angle in degrees for turning

//Function to configure ports to enable robot's motion
void PosEncoderMotionPinConfig () {
    DDRA = DDRA | 0x0F;
    PORTA = PORTA & 0xF0;
    DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
    PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to configure INT4 (PORTE 4) pin as input for the left position encoder
void LeftEncoderPinConfig () {
    DDRE  = DDRE & 0xEF;  //Set the direction of the PORTE 4 pin as input
    PORTE = PORTE | 0x10; //Enable internal pull-up for PORTE 4 pin
}

//Function to configure INT5 (PORTE 5) pin as input for the right position encoder
void RightEncoderPinConfig () {
    DDRE  = DDRE & 0xDF;  //Set the direction of the PORTE 4 pin as input
    PORTE = PORTE | 0x20; //Enable internal pull-up for PORTE 4 pin
}

//Function to initialize ports
void PosEncoderPortInit () {
    PosEncoderMotionPinConfig(); //robot motion pins config
    LeftEncoderPinConfig(); //left encoder pin config
    RightEncoderPinConfig(); //right encoder pin config	
}

//Interrupt 4 enable
void LeftPositionEncoderInterruptInit () {
    cli(); //Clears the global interrupt
    EICRB = EICRB | 0x02; // INT4 is set to trigger with falling edge
    EIMSK = EIMSK | 0x10; // Enable Interrupt INT4 for left position encoder
    sei();   // Enables the global interrupt 
}

//Interrupt 5 enable
void RightPositionEncoderInterruptInit () {
    cli(); //Clears the global interrupt
    EICRB = EICRB | 0x08; // INT5 is set to trigger with falling edge
    EIMSK = EIMSK | 0x20; // Enable Interrupt INT5 for right position encoder
    sei();   // Enables the global interrupt 
}

// Timer 5 initialized in PWM mode for velocity control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
void PosEncoderTimer5Init() {
    TCCR5B = 0x00;  //Stop
    TCNT5H = 0xFF;  //Counter higher 8-bit value to which OCR5xH value is compared with
    TCNT5L = 0x01;  //Counter lower 8-bit value to which OCR5xH value is compared with
    OCR5AH = 0x00;  //Output compare register high value for Left Motor
    OCR5AL = 0xFF;  //Output compare register low value for Left Motor
    OCR5BH = 0x00;  //Output compare register high value for Right Motor
    OCR5BL = 0xFF;  //Output compare register low value for Right Motor
    OCR5CH = 0x00;  //Output compare register high value for Motor C1
    OCR5CL = 0xFF;  //Output compare register low value for Motor C1
    TCCR5A = 0xA9;  /*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
              For Overriding normal port functionality to OCRnA outputs.
                {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
    TCCR5B = 0x0B;  //WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

//ISR for right position encoder
ISR(INT5_vect) {
    ShaftCountRight++;  //increment right shaft position count
}


//ISR for left position encoder
ISR(INT4_vect) {
    ShaftCountLeft++;  //increment left shaft position count
}


//Function used for setting motor's direction
void PosEncoderMotionSet (unsigned char Direction) {
    unsigned char PortARestore = 0; 
    Direction &= 0x0F; 		// removing upper nibbel for the protection
    PortARestore = PORTA; 		// reading the PORTA original status
    PortARestore &= 0xF0; 		// making lower direction nibbel to 0
    PortARestore |= Direction; // adding lower nibbel for forward command and restoring the PORTA status
    PORTA = PortARestore; 		// executing the command
}

// Function for robot velocity control
// Input must be in range of 0-255
void PosEncoderVelocity(unsigned char left_motor, unsigned char right_motor) {
    OCR5AL = (unsigned char)left_motor;
    OCR5BL = (unsigned char)right_motor;
}

//both wheels forward 
void PosEncoderForward () {
    PosEncoderMotionSet(0x06);
}

//both wheels backward
void PosEncoderBack () {
    PosEncoderMotionSet(0x09);
}

//Left wheel backward, Right wheel forward
void PosEncoderLeft () {
    PosEncoderMotionSet(0x05);
}

//Left wheel forward, Right wheel backward
void PosEncoderRight () 
{
  PosEncoderMotionSet(0x0A);
}

//Left wheel stationary, Right wheel forward
void PosEncoderSoftLeft () {
    PosEncoderMotionSet(0x04);
}

//Left wheel forward, Right wheel is stationary
void PosEncoderSoftRight () {
    PosEncoderMotionSet(0x02);
}

//Left wheel backward, right wheel stationary
void PosEncoderSoftLeft2 () {
    PosEncoderMotionSet(0x01);
}

//Left wheel stationary, Right wheel backward
void PosEncoderSoftRight2 () {
    PosEncoderMotionSet(0x08);
}

void PosEncoderStop () {
    PosEncoderMotionSet(0x00);
}


//Function used for turning robot by specified degrees
void PosEncoderAngleRotate(unsigned int degrees) {
    float reqd_shaft_count = 0;
    unsigned long int reqd_shaft_count_int = 0;    
    reqd_shaft_count = (float) degrees/ 4.090; // division by resolution to get shaft count
    reqd_shaft_count_int = (unsigned int) reqd_shaft_count;
    ShaftCountRight = 0; 
    ShaftCountLeft = 0;     
    while (1)
    {
     // lcd_printf("Right %d", ShaftCountRight);
     if((ShaftCountRight >= reqd_shaft_count_int) | (ShaftCountLeft >= reqd_shaft_count_int))
     break;
    }
     PosEncoderStop(); //Stop robot
}

//Function used for moving robot forward by specified distance
// If the motors move at different speeds, you want to pass the distance the faster motor needs to turn
void PosEncoderLinearDistanceMm(unsigned int distance_in_mm) {
    float reqd_shaft_count = 0;
    unsigned long int reqd_shaft_count_int = 0; 
    reqd_shaft_count = distance_in_mm / 5.338; // division by resolution to get shaft count
    reqd_shaft_count_int = (unsigned long int) reqd_shaft_count;
     
    ShaftCountRight = 0;
    ShaftCountLeft = 0;
    while(1)
    {
     // lcd_printf("Right %d", ShaftCountRight);
     if((ShaftCountRight > reqd_shaft_count_int) | (ShaftCountLeft > reqd_shaft_count_int))
     {
     	break;
     }
    } 
    PosEncoderStop(); //Stop robot
}

void PosEncoderForwardMm(unsigned int distance_in_mm) {
    PosEncoderForward();
    PosEncoderLinearDistanceMm(distance_in_mm);
}

void PosEncoderBackMm(unsigned int distance_in_mm) {
    PosEncoderBack();
    PosEncoderLinearDistanceMm(distance_in_mm);
}

void PosEncoderLeftDegrees(unsigned int Degrees) {
    // 88 pulses for 360 degrees rotation 4.090 degrees per count
    PosEncoderLeft(); //Turn left
    PosEncoderAngleRotate(Degrees);
}


void PosEncoderRightDegrees(unsigned int Degrees){
    // 88 pulses for 360 degrees rotation 4.090 degrees per count
    PosEncoderRight(); //Turn right
    PosEncoderAngleRotate(Degrees);
}


void PosEncoderSoftLeftDegrees(unsigned int Degrees) {
    // 176 pulses for 360 degrees rotation 2.045 degrees per count
    PosEncoderSoftLeft(); //Turn soft left
    Degrees=Degrees*2;
    PosEncoderAngleRotate(Degrees);
}

void PosEncoderSoftRightDegrees(unsigned int Degrees) {
    // 176 pulses for 360 degrees rotation 2.045 degrees per count
    PosEncoderSoftRight();  //Turn soft right
    Degrees=Degrees*2;
    PosEncoderAngleRotate(Degrees);
}

void PosEncoderSoftLeft2Degrees(unsigned int Degrees) {
    // 176 pulses for 360 degrees rotation 2.045 degrees per count
    PosEncoderSoftLeft2(); //Turn reverse soft left
    Degrees=Degrees*2;
    PosEncoderAngleRotate(Degrees);
}

void PosEncoderSoftRight2Degrees(unsigned int Degrees) {
    // 176 pulses for 360 degrees rotation 2.045 degrees per count
    PosEncoderSoftRight2();  //Turn reverse soft right
    Degrees=Degrees*2;
    PosEncoderAngleRotate(Degrees);
}

//Function to initialize all the devices
void PosEncoderInitDevices() {
    cli(); //Clears the global interrupt
    PosEncoderPortInit();  //Initializes all the ports
    LeftPositionEncoderInterruptInit();
    RightPositionEncoderInterruptInit();
    sei();   // Enables the global interrupt 
}


void PosEncoderRotateBot(int Degrees) {
    if (abs(180 - Degrees) < 5) {
        PosEncoderForwardMm(5);
    }
    if(Degrees > 0)
    {
      PosEncoderLeftDegrees(abs(Degrees));
      PosEncoderStop();     
      // _delay_ms(500); 
    }
    else {
      PosEncoderRightDegrees(abs(Degrees));
      PosEncoderStop();     
      // _delay_ms(500); 
    }
}

void ResetRightShaft() {
    ShaftCountRight = 0;
}

void ResetLeftShaft() {
    ShaftCountLeft = 0;
}

unsigned char GetShaftCountRight() {
    return ShaftCountRight;
}

unsigned char GetShaftCountLeft() {
    return ShaftCountLeft;
}