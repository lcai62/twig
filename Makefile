CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Wformat=2 -g
LDFLAGS = -lcrypto -lz

SOURCES := $(wildcard src/**/*.c src/*.c)
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
TARGET = twig

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
