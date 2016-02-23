CC=avr-gcc

OBJCOPY=avr-objcopy

CFLAGS=-g -Wall -DF_CPU=14745600 -mmcu=atmega2560

# Modify these 2 for each project as needed
FILENAME=seven_segment
DEPS=app.o

#Info til AVRDUDE
PROGRAMMER = avrdude
BOARD = stk500v2
MCU = m2560
PORT = /dev/tty.usbmodem1411

PFLAGS = -p $(MCU) -c $(BOARD) -P $(PORT)

rom.hex : app.out
	$(OBJCOPY) -j .text -j .data -O ihex app.out rom.hex

app.out : $(FILENAME).o app.o
	$(CC) $(CFLAGS) -o app.out $(FILENAME).o $(DEPS) -lm

%.o : %.c
	$(CC) $(CFLAGS) -Os -c $<

.PHONY: clean program
clean:
	rm -f *.o *.out *.map *.hex

program:
	$(PROGRAMMER) $(PFLAGS) -e
	$(PROGRAMMER) $(PFLAGS) -U flash:w:rom.hex
