CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=
SOURCES=main.c elwe.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=elwe

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
