CC=g++
#CFLAGS=-c -Wall -DTRACK -DORDERED
CFLAGS=-c -Wall -DORDERED

UTILS=utils.cpp
IO=io.cpp
MAIN=main.cpp

SOURCES=$(UTILS) $(IO) $(MAIN)
OBJECTS=$(SOURCES:.cpp=.o)

EXECUTABLE=db

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)
