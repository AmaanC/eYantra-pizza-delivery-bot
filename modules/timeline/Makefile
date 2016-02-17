CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c)) ../map/map.o ../dijkstra/dijkstra.o ../bot_memory/bot_memory.o ../timer-gcc/timer.o

$(FILENAME) : $(objects)
	cc -Wall -o $(FILENAME).out $(objects) -lm

clean:
	rm -f *.o *.out *.map *.hex

run: $(FILENAME)
	./app.out