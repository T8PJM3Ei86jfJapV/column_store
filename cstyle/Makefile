CC=gcc
CFLAGS=-c -Wall

UTILS=utils.c
IO=io.c
MAIN=main.c

SOURCES=$(UTILS) $(IO) $(MAIN)
OBJECTS=$(SOURCES:.c=.o)

EXECUTABLE=db

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)
