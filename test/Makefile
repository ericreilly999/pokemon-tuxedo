# Makefile for Pokemon Tuxedo property-based tests

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I../include
LDFLAGS =

# Source files
TEST_SOURCES = test_level_scaler.c
IMPL_SOURCES = ../src/level_scaler.c

# Object files
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
IMPL_OBJECTS = $(IMPL_SOURCES:.c=.o)

# Test executables
TEST_EXECUTABLES = test_level_scaler

.PHONY: all clean test

all: $(TEST_EXECUTABLES)

test_level_scaler: test_level_scaler.o ../src/level_scaler.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(TEST_EXECUTABLES)
	@echo "Running property-based tests..."
	@./test_level_scaler

clean:
	rm -f $(TEST_OBJECTS) $(IMPL_OBJECTS) $(TEST_EXECUTABLES)
	rm -f *.o ../src/*.o

.PHONY: test
