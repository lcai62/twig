CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS = -lcrypto -lz

SOURCES = src/main.c src/object.c src/repo.c src/utils.c src/cmd/cmd_init.c src/cmd/cmd_hash_object.c src/cmd/cmd_cat_file.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = twig

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
