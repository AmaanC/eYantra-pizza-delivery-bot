#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pos_encoder.h"
//#include "../lcd/lcd.h"

//Main Function

int main() {
	PosEncoderInitDevices();
	PosEncoderTimer5Init();
	//LcdInitDevices();
	//LcdSet4Bit();
	//LcdInit();
	//LcdPrintf("DANGER ZONE");
	while(1)
	{
		PosEncoderVelocity(255, 210);
		PosEncoderForward();
		PosEncoderLinearDistanceMm(300);
		_delay_ms(500);

		PosEncoderForwardMm(100); //Moves robot forward 100mm
		PosEncoderStop();
		_delay_ms(500);			
		
		PosEncoderBackMm(100);   //Moves robot backward 100mm
		PosEncoderStop();			
		_delay_ms(500);
		
		PosEncoderLeftDegrees(90); //Rotate robot left by 90 degrees
		PosEncoderStop();
		_delay_ms(500);
		
		PosEncoderRightDegrees(90); //Rotate robot right by 90 degrees
		PosEncoderStop();
		_delay_ms(500);
		
		PosEncoderSoftLeftDegrees(90); //Rotate (soft turn) by 90 degrees
		PosEncoderStop();
		_delay_ms(500);
		
		PosEncoderSoftRightDegrees(90);	//Rotate (soft turn) by 90 degrees
		PosEncoderStop();
		_delay_ms(500);

		PosEncoderSoftLeft2Degrees(90); //Rotate (soft turn) by 90 degrees
		PosEncoderStop();
		_delay_ms(500);
		
		PosEncoderSoftRight2Degrees(90);	//Rotate (soft turn) by 90 degrees
		PosEncoderStop();
		_delay_ms(500);
	}
}

