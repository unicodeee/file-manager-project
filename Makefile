CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror
TARGET := project
SOURCES := main.c directory.c storage.c commands.c
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c filesystem.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)
