#ifndef LCD_H_
#define LCD_H_

void LcdPortConfig();
void LcdPortInit();
void LcdInitDevices();
void LcdSet4Bit();
void LcdInit();
void LcdWrCommand(unsigned char cmd);
void LcdWrChar(char letter);
void LcdHome();
void LcdPrintf(const char *fmt, ...);
void LcdString(char *str);
void LcdCursor (char row, char column);
void LcdPrint (char row, char coloumn, unsigned int value, int digits);

#endif