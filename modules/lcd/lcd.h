#ifndef LCD_H
#define LCD_H

void lcd_port_config();
void lcd_port_init();
void lcd_init_devices();
void lcd_set_4bit();
void lcd_init();
void lcd_wr_command(unsigned char cmd);
void lcd_wr_char(char letter);
void lcd_home();
void lcd_printf(const char *fmt, ...);
void lcd_string(char *str);
void lcd_cursor (char row, char column);
void lcd_print (char row, char coloumn, unsigned int value, int digits);

#endif