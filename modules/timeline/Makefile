CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c)) ../map/map.o ../dijkstra/dijkstra.o ../bot_memory/bot_memory.o ../timer-gcc/timer.o ../hardware_control/hardware_control.o

$(FILENAME) : $(objects)
	cc -o $(FILENAME).out $(objects) -lm

%.o : %.c
	$(CC) -Wall -Os -c $< -o $(patsubst %.c,%.o,$<)

clean:
	find .. -type f -name '*.o' -exec rm {} +

run: $(FILENAME)
	./app.out