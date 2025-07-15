# Makefile para mlab0-ex01-fibonacci.c

CC = gcc
CFLAGS = -Wall -pthread
TARGET = mlab0-ex01-fibonacci
SRC = mlab0-ex01-fibonacci.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
