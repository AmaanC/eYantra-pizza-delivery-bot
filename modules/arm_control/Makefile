CC=avr-gcc

OBJCOPY=avr-objcopy

CFLAGS=-g -Wall -DF_CPU=14745600 -mmcu=atmega2560

# Modify these 2 for each project as needed
FILENAME=arm_control
DEPS=app.o ../map/map.o ../buzzer/buzzer.o ../color_sensor/color_sensor.o ../sharp_sensor/sharp_sensor.o ../rgb_led/rgb_led.o ../bot_memory/bot_memory.o ../servo/servo.o ../move_bot/move_bot.o ../pos_encoder/pos_encoder.o ../bl_sensor/bl_sensor.o ../timeline-avr/timeline.o ../timer/timer.o ../custom_delay/custom_delay.o ../hardware_control-avr/hardware_control.o ../dijkstra/dijkstra.o ../lcd/lcd.o ../timeline-avr/helpers.o ../seven_segment/seven_segment.o

#Info til AVRDUDE
PROGRAMMER = avrdude
BOARD = stk500v2
MCU = m2560
PORT = /dev/tty.usbmodem1411

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