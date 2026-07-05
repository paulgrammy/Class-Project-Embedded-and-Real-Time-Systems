CC = gcc
CFLAGS = -O2
LDFLAGS = -lncursesw

SRC = uartscreen.c
INC = unicode_subs.h
OUT = UARTscreen.elf

all: $(OUT)

$(OUT): $(SRC) $(INC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)
 
