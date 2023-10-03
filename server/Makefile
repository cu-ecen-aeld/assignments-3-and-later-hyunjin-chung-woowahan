CFLAGS ?= -g -Wall -Werror
TARGET ?= aesdsocket
LDFLAGS ?= -pthread -lrt
CC ?= $(CROSS_COMPILE)gcc
SRCS = aesdsocket.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET) 

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
