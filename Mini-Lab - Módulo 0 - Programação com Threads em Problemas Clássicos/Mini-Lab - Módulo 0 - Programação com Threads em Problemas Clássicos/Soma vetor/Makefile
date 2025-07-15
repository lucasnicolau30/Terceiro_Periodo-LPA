# Makefile para mlab0-ex03-soma_vetor.c

CC = gcc
CFLAGS = -Wall -pthread
TARGET = mlab0-ex03-soma_vetor
SRC = mlab0-ex03-soma_vetor.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
