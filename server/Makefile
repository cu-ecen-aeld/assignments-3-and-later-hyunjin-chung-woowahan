CFLAGS ?= -g -Wall -Werror
TARGET ?= aesdsocket
LDFLAGS ?= -lpthread
CC ?= $(CROSS_COMPILE)gcc
SRCS = aesdsocket.c
OBJS = $(SRCS:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

all: $(TARGET) 

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
