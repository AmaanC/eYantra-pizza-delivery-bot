CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c)) ../map/map.o ../bot_memory/bot_memory.o

$(FILENAME) : $(objects)
	cc -o $(FILENAME).out $(objects) -lm

clean:	
	find .. -type f -name '*.o' -exec rm {} +

run: $(FILENAME)
	./app.out