CC = gcc
CFLAGS = -Wall -Wextra -std=gnu17 -O2
LDFLAGS = -lm

SRCS = main.c utils.c sent.c receive.c
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