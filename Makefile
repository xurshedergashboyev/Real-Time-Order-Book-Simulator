CC = gcc
CFLAGS = -Wall -pthread
TARGET = orderbook

all: $(TARGET)

$(TARGET): main.c order_book.c order_book.h
	$(CC) $(CFLAGS) -o $(TARGET) main.c order_book.c

clean:
	rm -f $(TARGET)
