CC = gcc
CFLAGS = -Wall -Wextra -std=gnu17
LDFLAGS = -lm

SRCS = sent.c receive.c main.c
OBJS = $(SRCS:.c=.o)
TARGET = traceroute

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)