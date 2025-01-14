CC = gcc
CFLAGS = -Wall -Wextra -D_BSD_SOURCE -D_GNU_SOURCE

TARGET = ish

SRCS = main.c dynarray.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)