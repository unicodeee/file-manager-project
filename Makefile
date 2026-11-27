CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -Werror
DEBUG_FLAGS := -g -O0
TARGET := project
SOURCES := main.c directory.c storage.c commands.c
OBJECTS := $(SOURCES:.c=.o)

.PHONY: all clean run

all: $(TARGET)

# Add this rule to rebuild with debug symbols
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c filesystem.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJECTS)
