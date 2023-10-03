CFLAGS ?= -g -Wall -Werror
TARGET ?= aesdsocket
LDFLAGS ?= -lpthread -lrt
CC ?= $(CROSS_COMPILE)gcc
OBJS = aesdsocket.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

.o.c:
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES) $(LDFLAGS)

all: $(TARGET) 

clean:
	rm -f $(OBJS) $(TARGET)
