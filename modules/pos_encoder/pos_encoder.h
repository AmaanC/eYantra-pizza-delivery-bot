#ifndef POSENCODER_H_
#define POSENCODER_H_

void PosEncoderMotionPinConfig(); 
void LeftEncoderPinConfig();
void RightEncoderPinConfig();
void PosEncoderPortInit();
void LeftPositionEncoderInterruptInit();
void RightPositionEncoderInterruptInit(); 
void PosEncoderTimer5Init();
void PosEncoderVelocity(unsigned char leftMotor, unsigned char rightMotor);
void PosEncoderMotionSet (unsigned char Direction);
void PosEncoderForward();
void PosEncoderBack(); 
void PosEncoderLeft();
void PosEncoderRight();
void PosEncoderSoftLeft(); 
void PosEncoderSoftRight(); 
void PosEncoderSoftLeft2(); 
void PosEncoderSoftRight2(); 
void PosEncoderStop();
void PosEncoderAngleRotate(unsigned int Degrees);
void PosEncoderLinearDistanceMm(unsigned int DistanceInMM);
void PosEncoderForwardMm(unsigned int DistanceInMM);
void PosEncoderBackMm(unsigned int DistanceInMM);
void PosEncoderLeftDegrees(unsigned int Degrees) ;
void PosEncoderRightDegrees(unsigned int Degrees);
void PosEncoderSoftLeftDegrees(unsigned int Degrees);
void PosEncoderSoftRightDegrees(unsigned int Degrees);
void PosEncoderSoftLeft2Degrees(unsigned int Degrees);
void PosEncoderSoftRight2Degrees(unsigned int Degrees);
void PosEncoderInitDevices();
void PosEncoderRotateBot(int Degrees);

void ResetRightShaft();
void ResetLeftShaft();

unsigned char GetShaftCountRight();
unsigned char GetShaftCountLeft();


#endif