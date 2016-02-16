#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Function to configure ports to enable robot's motion
void MotionPinConfig() {
    DDRA = DDRA | 0x0F;
    PORTA = PORTA & 0xF0;
    DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
    PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to initialize ports
void InitPorts() {
    MotionPinConfig();
}

// Timer 5 initialized in PWM mode for velocity control
// Prescale:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
void Timer5Init() {
    TCCR5B = 0x00;	//Stop
    TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
    TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
    OCR5AH = 0x00;	//Output compare register high value for Left Motor
    OCR5AL = 0xFF;	//Output compare register low value for Left Motor
    OCR5BH = 0x00;	//Output compare register high value for Right Motor
    OCR5BL = 0xFF;	//Output compare register low value for Right Motor
    OCR5CH = 0x00;	//Output compare register high value for Motor C1
    OCR5CL = 0xFF;	//Output compare register low value for Motor C1
    TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
    TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

// Function for robot velocity control
void Velocity (unsigned char left_motor, unsigned char right_motor) {
    OCR5AL = (unsigned char)left_motor;
    OCR5BL = (unsigned char)right_motor;
}

//Function used for setting motor's direction
void MotionSet (unsigned char Direction) {
    unsigned char portA_restore = 0;

    Direction &= 0x0F; 			// removing upper nibbel as it is not needed
    portA_restore = PORTA; 			// reading the PORTA's original status
    portA_restore &= 0xF0; 			// setting lower direction nibbel to 0
    portA_restore |= Direction; 	// adding lower nibbel for direction command and restoring the PORTA status
    PORTA = portA_restore; 			// setting the command to the port
}


void Forward() { //both wheels forward
    MotionSet(0x06);
}

void Back() { //both wheels backward
    MotionSet(0x09);
}

void Left() { //Left wheel backward, Right wheel forward
    MotionSet(0x05);
}

void Right() { //Left wheel forward, Right wheel backward
    MotionSet(0x0A);
}

void SoftLeft() { //Left wheel stationary, Right wheel forward
    MotionSet(0x04);
}

void SoftRight() { //Left wheel forward, Right wheel is stationary
    MotionSet(0x02);
}

void SoftLeft2() { //Left wheel backward, right wheel stationary
    MotionSet(0x01);
}

void SoftRight2() { //Left wheel stationary, Right wheel backward
    MotionSet(0x08);
}

void Stop() {
    MotionSet(0x00);
}

void InitDevices() { //use this function to initialize all devices
    cli(); //disable all interrupts
    init_ports();
    timer5_init();
    sei(); //re-enable interrupts
}

//Main Function
int main() {
    init_devices();
    velocity (126.25905331, 153.89310331); //Set robot velocity here. Smaller the value lesser will be the velocity
						 //Try different valuse between 0 to 255
	while(1) {
	    
            forward(); //both wheels forward
            _delay_ms(30000);

            stop();                     
            _delay_ms(1000);
	}
}