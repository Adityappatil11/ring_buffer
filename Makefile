CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -g

SRC_DIR = src
TEST_DIR = tests
INC_DIR = include

SRCS = $(SRC_DIR)/ring_buffer.c $(TEST_DIR)/test_ring_buffer.c 
TARGET = test_runner

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)