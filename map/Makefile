CC=gcc
FILENAME=app

objects := $(patsubst %.c,%.o,$(wildcard *.c))

$(FILENAME) : $(objects)
	cc -o $(FILENAME).out $(objects)

clean:
	rm -f *.o *.out *.map *.hex

run:
	./app.out