CC_FLAGS=-c -Wall -Werror
OBJS = aesdsocket.o
TARGET = aesdsocket

ifdef CROSS_COMPILE
	CC = $(CROSS_COMPILE)gcc
else
	CC = gcc
endif

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -lpthread

$(OBJS) : aesdsocket.c
	$(CC) $(CC_FLAGS) aesdsocket.c -o $(OBJS)

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)
