//#define __OPTIMIZE__ -O0
#define F_CPU 14745600

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <math.h> //included to support power function

void seven_segment_pin_config()
{
    // Port D upper nibble for CA connections
    // Port J for a,b,c,d, e,f,g,DEC as per manual
    // Both things above are in decreasing order
    // i.e.
    // Port J = Pin 7, 6, 5...0

    DDRD = DDRD | 0xF0; // set upper nibble of port D to output
    DDRJ = DDRJ | 0xFF; // set all bits to output
}

//ADC pin configuration
void adc_pin_config()
{
	DDRF = 0x00; //set PORTF direction as input
	PORTF = 0x00; //set PORTF pins floating
	DDRK = 0x00; //set PORTK direction as input
	PORTK = 0x00; //set PORTK pins floating
}

void port_init()
{
    seven_segment_pin_config();
	adc_pin_config();
}

//Function to Initialize ADC
void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

void init_devices()
{
    cli(); //Clears the global interrupt
    port_init();  //Initializes all the ports
	adc_init();
    sei();   // Enables the global interrupt
}


//This Function accepts the Channel Number and returns the corresponding Analog Value 
unsigned char ADC_Conversion(unsigned char Ch)
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for ADC conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}

// This Function calculates the actual distance in millimeters(mm) from the input
// analog value of Sharp Sensor. 
unsigned int Sharp_GP2D12_estimation(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
}

int convert_to_hex(unsigned int num) {
    int ret = 0x00;
    switch(num) {
        case 0:
            ret = 0x03;
            break;
        case 1:
            ret = 0x9F;
            break;
        case 2:
            ret = 0x25;
            break;
        case 6:
            ret = 0x41;
            break;
        case 9:
            ret = 0x09;
            break;
    }
    return ret;
}


void display_num(unsigned int num) {
    int digit;
    int MIN_SELECT_VALUE = 0x10;
    int MAX_SELECT_VALUE = 0x40;

    int select_value = MIN_SELECT_VALUE; // Initially 0x10. Refer to pins.txt for other values (they just double)
    PORTD = PORTD & 0x0F; // Reset upper nibble to 0
    PORTD = PORTD | select_value;

    while (select_value <= MAX_SELECT_VALUE) {
        digit = num % 10;
        PORTJ = convert_to_hex(digit);
        _delay_us(500);
        select_value *= 2;
        if (select_value <= MAX_SELECT_VALUE) {
            PORTD = PORTD & 0x0F; // Reset upper nibble to 0
            PORTD = PORTD | select_value; // Set upper nibble value to select apt CA
        }
        num /= 10;
    }
}

// default_dist is used when we can't tell what block we're detecting 
// because it is between two threshold values (small+threshold and medium-threshold)
int get_block_size(int distance, int default_dist) {
    int threshold = 13;
	int no_block_threshold = 16;
	// All experimental averages
    int no_block_height = 320;
    int small_height = 262;
    int medium_height = 234;
    int large_height = 204;

    int ret = 0;

    if (fabs(distance - no_block_height) < no_block_threshold) {
        ret = 0;
    }
    else if(fabs(distance - small_height) < threshold) {
        ret = 6;
    }
    else if(fabs(distance - medium_height) < threshold) {
        ret = 9;
    }
    else if(fabs(distance - large_height) < threshold) {
        ret = 12;
    }
	else {
		ret = default_dist;
	}

    return ret;
}

int main()
{
    int sharp;
    int value;
    int block_size = 0;
    init_devices();
    while(1) {
        sharp = ADC_Conversion(11);
        value = Sharp_GP2D12_estimation(sharp);
        block_size = get_block_size(value, block_size); // Get block size from sharp sensor distance (in mm)

        display_num(block_size);
        _delay_us(500);
    }
}
