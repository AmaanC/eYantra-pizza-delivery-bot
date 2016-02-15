CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c)) ../map/map.o

$(FILENAME) : $(objects)
	cc -o $(FILENAME).out $(objects) -lm

clean:
	rm -f *.o *.out *.map *.hex

run: $(FILENAME)
	./app.out