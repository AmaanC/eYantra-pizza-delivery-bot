CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c))

$(FILENAME) : $(objects)
	cc -o $(FILENAME).out $(objects) -lm

clean:
	rm -f *.o *.out *.map *.hex

run: $(FILENAME)
	./app.out