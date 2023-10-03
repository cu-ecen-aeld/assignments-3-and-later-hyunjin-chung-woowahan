CFLAGS ?= -c -Wall -Werror
TARGET ?= aesdsocket
LDFLAGS ?= -lpthread -lrt
CC ?= $(CROSS_COMPILE)gcc

$(TARGET): aesdsocket.c
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

all: $(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
