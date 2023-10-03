CFLAGS ?= -g -Wall -Werror
TARGET ?= aesdsocket
LDFLAGS ?= -lpthread -lrt
CC ?= $(CROSS_COMPILE)gcc
OBJS = aesdsocket.o
SRCS = aesdsocket.c

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $^ -o $@ $(INCLUDES) $(LDFLAGS)

all: $(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
