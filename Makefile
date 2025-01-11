CC = gcc
CFLAGS = -Wall -Wextra -O2
DFLAGS = -g -O0
TARGET = lpf
DTARGET = lpf_debug
SRC = main.c lpf.c suffix_array.c suffix_array_naive.c tuple.c radix.c

all: $(TARGET)

debug:
	$(CC) $(DFLAGS) -o $(DTARGET) $(SRC)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
