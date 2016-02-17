/********************************************************************************

 LCD Connections:
 			 LCD	  Microcontroller Pins
 			  RS  --> PC0
			  RW  --> PC1
			  EN  --> PC2
			  DB7 --> PC7
			  DB6 --> PC6
			  DB5 --> PC5
			  DB4 --> PC4

 Note: 
 
 1. Make sure that in the configuration options following settings are 
 	done for proper operation of the code

 	Microcontroller: atmega2560
 	Frequency: 14745600
 	Optimization: -O0 (For more information read section: Selecting proper optimization 
 					options below figure 2.22 in the Software Manual)

 2. Buzzer is connected to PC3. Hence to operate buzzer without interfering with the LCD, 
 	buzzer should be turned on or off only using buzzer function 
********************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

#include <stdarg.h>

#define RS 0
#define RW 1
#define EN 2
#define Lcdport PORTC

#define sbit(reg,bit)	reg |= (1<<bit)			// Macro defined for Setting a bit of any register.
#define cbit(reg,bit)	reg &= ~(1<<bit)		// Macro defined for Clearing a bit of any register.

unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;

//Function to configure LCD port
void LcdPortConfig (void) {
    DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
    PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//Function to Initialize PORTS
void LcdPortInit() {
	LcdPortConfig();
}

void LcdInitDevices (void) {
    cli(); //Clears the global interrupts
    LcdPortInit();
    sei();   //Enables the global interrupts
}

//Function to Reset LCD
void LcdSet4Bit() {
	_delay_ms(1);

	cbit(Lcdport,RS);				//RS=0 --- Command Input
	cbit(Lcdport,RW);				//RW=0 --- Writing to LCD
	Lcdport = 0x30;				//Sending 3
	sbit(Lcdport,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(Lcdport,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(Lcdport,RS);				//RS=0 --- Command Input
	cbit(Lcdport,RW);				//RW=0 --- Writing to LCD
	Lcdport = 0x30;				//Sending 3
	sbit(Lcdport,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(Lcdport,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(Lcdport,RS);				//RS=0 --- Command Input
	cbit(Lcdport,RW);				//RW=0 --- Writing to LCD
	Lcdport = 0x30;				//Sending 3
	sbit(Lcdport,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(Lcdport,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(Lcdport,RS);				//RS=0 --- Command Input
	cbit(Lcdport,RW);				//RW=0 --- Writing to LCD
	Lcdport = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(Lcdport,EN);				//Set Enable Pin
	_delay_ms(1);					//Delay
	cbit(Lcdport,EN);				//Clear Enable Pin

	
}

//Function to Initialize LCD
void LcdInit() {
	_delay_ms(1);

	LcdWrCommand(0x28);			//LCD 4-bit mode and 2 lines.
	LcdWrCommand(0x01);
	LcdWrCommand(0x06);
	LcdWrCommand(0x0E);
	LcdWrCommand(0x80);
		
}

	 
//Function to Write Command on LCD
void LcdWrCommand(unsigned char cmd) {
	unsigned char temp;
	temp = cmd;
	temp = temp & 0xF0;
	Lcdport &= 0x0F;
	Lcdport |= temp;
	cbit(Lcdport,RS);
	cbit(Lcdport,RW);
	sbit(Lcdport,EN);
	_delay_ms(5);
	cbit(Lcdport,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	Lcdport &= 0x0F;
	Lcdport |= cmd;
	cbit(Lcdport,RS);
	cbit(Lcdport,RW);
	sbit(Lcdport,EN);
	_delay_ms(5);
	cbit(Lcdport,EN);
}

//Function to Write Data on LCD
void LcdWrChar(char letter) {
	char temp;
	temp = letter;
	temp = (temp & 0xF0);
	Lcdport &= 0x0F;
	Lcdport |= temp;
	sbit(Lcdport,RS);
	cbit(Lcdport,RW);
	sbit(Lcdport,EN);
	_delay_ms(5);
	cbit(Lcdport,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	Lcdport &= 0x0F;
	Lcdport |= letter;
	sbit(Lcdport,RS);
	cbit(Lcdport,RW);
	sbit(Lcdport,EN);
	_delay_ms(5);
	cbit(Lcdport,EN);
}


//Function to bring cursor at home position
void LcdHome(){
	LcdWrCommand(0x80);
}

void LcdPrintf(const char *fmt, ...) {
    va_list argp;
    const int LCDLEN = 16;
    char str[LCDLEN];
    va_start(argp, fmt);

    vsnprintf(str, LCDLEN, fmt, argp);

    va_end(argp);

    LcdString(str);
}


//Function to Print String on LCD
void LcdString(char *str) {
	LcdWrCommand(0x01); // Clear the LCD display
	while(*str != '\0')
	{
		LcdWrChar(*str);
		str++;
	}
}

//Position the LCD cursor at "row", "column".

void LcdCursor (char row, char column) {
	switch (row) {
		case 1: LcdWrCommand (0x80 + column - 1); break;
		case 2: LcdWrCommand (0xc0 + column - 1); break;
		case 3: LcdWrCommand (0x94 + column - 1); break;
		case 4: LcdWrCommand (0xd4 + column - 1); break;
		default: break;
	}
}

//Function To Print Any input value upto the desired digit on LCD
void LcdPrint (char row, char coloumn, unsigned int value, int digits) {
	unsigned char flag=0;
	if(row==0||coloumn==0)
	{
		LcdHome();
	}
	else
	{
		LcdCursor(row,coloumn);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		LcdWrChar(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		LcdWrChar(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		LcdWrChar(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		LcdWrChar(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		LcdWrChar(unit);
	}
	if(digits>5)
	{
		LcdWrChar('E');
	}
	
}
		