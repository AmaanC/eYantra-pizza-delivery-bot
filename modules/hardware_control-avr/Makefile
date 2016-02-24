CC=avr-gcc

OBJCOPY=avr-objcopy

CFLAGS=-g -DF_CPU=14745600 -mmcu=atmega2560

# Modify these 2 for each project as needed
FILENAME=hardware_control
DEPS=app.o ../map/map.o ../move_bot/move_bot.o ../bl_sensor/bl_sensor.o ../dijkstra/dijkstra.o ../bot_memory/bot_memory.o ../pos_encoder/pos_encoder.o ../lcd/lcd.o ../fake_gcc_fns/fake_gcc_fns.o ../timer/timer.o ../seven_segment/seven_segment.o

#Info til AVRDUDE
PROGRAMMER = avrdude
BOARD = stk500v2
MCU = m2560
PORT = /dev/ttyACM0

PFLAGS = -p $(MCU) -c $(BOARD) -P $(PORT)

rom.hex : app.out
	$(OBJCOPY) -j .text -j .data -O ihex app.out rom.hex

app.out : $(FILENAME).o $(DEPS)
	$(CC) $(CFLAGS) -o app.out -Wl,-Map,$(FILENAME).map $(FILENAME).o $(DEPS) -lm

%.o : %.c
	$(CC) $(CFLAGS) -Os -c $< -o $(patsubst %.c,%.o,$<) -lm

# rm -f *.o *.out *.map *.hex
clean:
	find .. -type f -name '*.o' -exec rm {} +

program:
	$(PROGRAMMER) $(PFLAGS) -e
	$(PROGRAMMER) $(PFLAGS) -U flash:w:rom.hex
