CC = gcc
CFLAGS = -Wall -Wextra -O3
DFLAGS = -g -O0
TARGET = lpf
DTARGET = lpf_debug
SRC = $(wildcard src/*.c)

all: $(TARGET)

debug:
	$(CC) $(DFLAGS) -o $(DTARGET) $(SRC)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
