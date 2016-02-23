CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c)) ../map/map.o ../dijkstra/dijkstra.o ../bot_memory/bot_memory.o #../pos_encoder/pos_encoder.o ../lcd/lcd.o

$(FILENAME) : $(objects)
	cc -g3 -o $(FILENAME).out $(objects) -lm

%.o : %.c
	$(CC) -g3 -Wall -Os -c $< -o $(patsubst %.c,%.o,$<)

clean:
	find .. -type f -name '*.o' -exec rm {} +

run: $(FILENAME)
	./app.out