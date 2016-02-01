#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Configure PORTB 5 pin for servo motor 1 operation
void servo_1_pin_config() {
 DDRB  = DDRB | 0x20;  //making PORTB 5 pin output
 PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
}

//Configure PORTB 6 pin for servo motor 2 operation
void servo_2_pin_config() {
 DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
 PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
}

//Configure PORTB 7 pin for servo motor 3 operation
void servo_3_pin_config() {
 DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
 PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
}

//Configure PORTH 4 pin for servo motor 4 operation
void servo_4_pin_config() {
 DDRH  = DDRH | 0x10;  // making PORTH 4 pin output
 PORTH = PORTH | 0x10; // setting PORTH 4 pin to logic 1
}

//Initialize the ports
void servo_port_init() {
 servo_1_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
 servo_2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation 
 servo_3_pin_config(); //Configure PORTB 7 pin for servo motor 3 operation  
 servo_4_pin_config(); //Configure PORTH 4 pin for servo motor 3 operation  
}

/*
And so we begin.
Welcome to Zombo. You can do anything. Welcome! To ZOMBOCOM!

Refer to the ATMEGA2560 user guide for more detailed references.

Here's my understanding, though:

"The 8-bit comparator continuously compares TCNT0 with the Output Compare Registers
(OCR0A and OCR0B)"
When they match, it triggers an interrupt. In the fast PWM mode we use, this causes the
PWM pulse to go to a logical 0 signal. The pulse goes back to a logical 1 signal
when the TCNT0 value hits 0x03FF for our mode.

The timer TOP value controls the frequency of the wave.
The OCRnx values determine when the PWM's pulse is dropped to 0. (Refer to the diagram)
So if OCRnx is set to 0x0000, it'll give a 0% duty cycle.
If OCRnx is set to TOP(0x03FF), it'll give a 100% duty cycle. 
TCNT1 below counts from 0xFC01 to 0xFFFF. When it matches 0x03FF(our TOP values), it is reset and starts counting again.
We control the servos by modifying the respective servos' OCR1X(lower nibble) values. 
Read section 16.7.3 about fast PWM mode
And section 16.9.1
And section 17.11.1

We set COM1A1=1 and COM1A0=0
According to the table, this clears OC0 on output compare match
Which is why the servos probably reset to 0 degrees every cycle.

Our WGM=111, so the TOP value for the timer is 0x03FF(according to table 17-2, p148, entry 7) */

//TIMER1 initialization in 10 bit fast PWM mode  
//prescale:256
// WGM: 7) PWM 10bit fast, TOP=0x03FF
// actual value: 52.25Hz 
void servo_timer_init() {
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
 TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
 OCR1AH = 0x03;	//Output compare Register high value for servo 1
 OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
 OCR1BH = 0x03;	//Output compare Register high value for servo 2
 OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
 OCR1CH = 0x03;	//Output compare Register high value for servo 3
 OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
 ICR1H  = 0x03;	
 ICR1L  = 0xFF;
 TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 					For Overriding normal port functionality to OCRnA outputs.
                    Clears the OCnA/B/C on compare match
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
 TCCR1C = 0x00;
 TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0(Prescaler=256) 
 // Set up timer 4 for the 4th servo. Output on OC4B, PH4
 TCCR4B = 0x00;
 
 TCNT4H = 0xFC;
 TCNT4L = 0x01;

 OCR4BH = 0x03;
 OCR4BL = 0xFF;

 ICR4H  = 0x03;
 ICR4L  = 0xFF;

 TCCR4A = 0xAB;
 TCCR4C = 0x00;
 TCCR4B = 0x0C;
}


//Function to initialize all the peripherals
void servo_init_devices() {
 cli(); //disable all interrupts
 servo_port_init();
 servo_timer_init();
 sei(); //re-enable interrupts 
}

// Returns an unsigned char because that type requires 1 byte(a max value of 0xFF) // which is exactly the amount of memory an OCR register's upper or lower nibble can accomodate
unsigned char servo_convert_deg(unsigned char degrees) {
    float ServoPosition = 0;
    ServoPosition = ((float) degrees / 1.86) + 35.0;
    return ServoPosition;
}

//Function to rotate Servo 1 by a specified angle in the multiples of 1.86 degrees
void servo_1_to(unsigned char degrees)  
{
 OCR1AH = 0x00;
 OCR1AL = (unsigned char) servo_convert_deg(degrees);
}


//Function to rotate Servo 2 by a specified angle in the multiples of 1.86 degrees
void servo_2_to(unsigned char degrees) {
 OCR1BH = 0x00;
 OCR1BL = (unsigned char) servo_convert_deg(degrees);
}

//Function to rotate Servo 3 by a specified angle in the multiples of 1.86 degrees
void servo_3_to(unsigned char degrees) {
 OCR1CH = 0x00;
 OCR1CL = (unsigned char) servo_convert_deg(degrees);
}

//Function to rotate Servo 3 by a specified angle in the multiples of 1.86 degrees
void servo_4_to(unsigned char degrees) {
 OCR4BH = 0x00;
 OCR4BL = (unsigned char) servo_convert_deg(degrees);
}

//servo_free functions unlocks the servo motors from the any angle 
//and make them free by giving 100% duty cycle at the PWM. This function can be used to 
//reduce the power consumption of the motor if it is holding load against the gravity.

//makes servo 1 free rotating
void servo_1_free() {
 OCR1AH = 0x03; 
 OCR1AL = 0xFF; //Servo 1 off
}

//makes servo 2 free rotating
void servo_2_free() {
 OCR1BH = 0x03;
 OCR1BL = 0xFF; //Servo 2 off
}

//makes servo 3 free rotating
void servo_3_free() {
 OCR1CH = 0x03;
 OCR1CL = 0xFF; //Servo 3 off
}

//makes servo 4 free rotating
void servo_4_free() {
 OCR4BH = 0x03;
 OCR4BL = 0xFF; //Servo 4 off
}