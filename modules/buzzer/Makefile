CC=avr-gcc

OBJCOPY=avr-objcopy

CFLAGS=-g -DF_CPU=14745600 -mmcu=atmega2560

# Modify these 2 for each project as needed
FILENAME=buzzer
DEPS=app.o ../custom_delay/custom_delay.o

#Info til AVRDUDE
PROGRAMMER = avrdude
BOARD = stk500v2
MCU = m2560
PORT = /dev/ttyACM0

PFLAGS = -p $(MCU) -c $(BOARD) -P $(PORT)

rom.hex : app.out
	$(OBJCOPY) -j .text -j .data -O ihex app.out rom.hex

app.out : $(FILENAME).o app.o
	$(CC) $(CFLAGS) -o app.out -Wl,-Map,$(FILENAME).map $(FILENAME).o $(DEPS)

%.o : %.c
	$(CC) $(CFLAGS) -Os -c $<

clean:
	rm -f *.o *.out *.map *.hex

program:
	$(PROGRAMMER) $(PFLAGS) -e
	$(PROGRAMMER) $(PFLAGS) -U flash:w:rom.hex