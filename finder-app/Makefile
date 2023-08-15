CC_FLAGS=-c
OBJS = writer.o
TARGET = writer

ifdef CROSS_COMPILE
	CC = $(CROSS_COMPILE)gcc
else
	CC = gcc
endif

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) 

$(OBJS) : writer.c
	$(CC) $(CC_FLAGS) writer.c -o $(OBJS)

all: $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)
