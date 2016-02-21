CC=avr-gcc

OBJCOPY=avr-objcopy

CFLAGS=-g -DF_CPU=14745600 -mmcu=atmega2560

# Modify these 2 for each project as needed
FILENAME=sense_pizza
DEPS=app.o ../color_sensor/color_sensor.o  ../sharp_sensor/SharpSensor.o ../lcd/lcd.o

#Info til AVRDUDE
PROGRAMMER = avrdude
BOARD = stk500v2
MCU = m2560
PORT = /dev/tty.usbmodem1421

PFLAGS = -p $(MCU) -c $(BOARD) -P $(PORT)

rom.hex : app.out
	$(OBJCOPY) -j .text -j .data -O ihex app.out rom.hex

app.out : $(FILENAME).o $(DEPS)
	$(CC) $(CFLAGS) -o app.out -Wl,-Map,$(FILENAME).map $(FILENAME).o $(DEPS) -lm

%.o : %.c
	$(CC) $(CFLAGS) -Os -c $< -o $(patsubst %.c,%.o,$<)

# rm -f *.o *.out *.map *.hex
clean:
	find .. -type f -name '*.o' -exec rm {} +

program:
	$(PROGRAMMER) $(PFLAGS) -e
	$(PROGRAMMER) $(PFLAGS) -U flash:w:rom.hex