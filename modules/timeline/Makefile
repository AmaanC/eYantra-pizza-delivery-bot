CC=avr-gcc

OBJCOPY=avr-objcopy

<<<<<<< HEAD
CFLAGS=-g -DF_CPU=14745600 -mmcu=atmega2560

# Modify these 2 for each project as needed
FILENAME=timeline
DEPS=app.o ../map/map.o ../dijkstra/dijkstra.o ../bot_memory/bot_memory.o ../timer-gcc/timer.o ../hardware_control-avr/hardware_control.o

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
=======
$(FILENAME) : $(objects)
	cc -g3 -o $(FILENAME).out $(objects) -lm

%.o : %.c
	$(CC) -g3 -Wall -Os -c $< -o $(patsubst %.c,%.o,$<)
>>>>>>> e25ff6c4da9f111d9dcb06c1d3d72c60dee0b858

# rm -f *.o *.out *.map *.hex
clean:
	find .. -type f -name '*.o' -exec rm {} +

program:
	$(PROGRAMMER) $(PFLAGS) -e
	$(PROGRAMMER) $(PFLAGS) -U flash:w:rom.hex
