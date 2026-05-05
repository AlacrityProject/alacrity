TARGET = main

CC = gcc

CFLAGS = -Wall -g

SRCS = $(wildcard src/*.c)

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) src/*.o

run: $(TARGET)
	./$(TARGET)