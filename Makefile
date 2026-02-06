# Simple universal Makefile for compiling C files to bin directory

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LIBS = -lpthread
SRCDIR = .
BINDIR = bin

# Automatically find all .c files in the current directory
SOURCES := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(SOURCES:.c=.o)
TARGET := $(BINDIR)/server

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(BINDIR):
	mkdir -p $(BINDIR)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f $(TARGET)

# Preserve HTML files and other assets in bin directory
.PRECIOUS: $(BINDIR)/%.html