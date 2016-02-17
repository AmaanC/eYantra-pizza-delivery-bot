CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c)) ../map/map.o ../dijkstra/dijkstra.o ../bot_memory/bot_memory.o #../pos_encoder/pos_encoder.o ../lcd/lcd.o

$(FILENAME) : $(objects)
	cc -o $(FILENAME).out $(objects) -lm

clean:
	rm -f *.o *.out *.map *.hex

run: $(FILENAME)
	./app.out