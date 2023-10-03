CFLAGS ?= -c -g -Wall -Werror
TARGET ?= aesdsocket
LDFLAGS ?= -lpthread -lrt
CC ?= $(CROSS_COMPILE)gcc
OBJS = aesdsocket.o

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(INCLUDES) $(LDFLAGS)

.c.o :
	$(CC) $(CFLAGS) $< -o $@ $(INCLUDES) $(LDFLAGS)

all: $(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
