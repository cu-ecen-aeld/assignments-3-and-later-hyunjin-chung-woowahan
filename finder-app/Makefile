OBJS = writer.o
TARGET = writer

ifdef CROSS_COMPILE
	CC = aarch64-none-linux-gnu-gcc
else
	CC = gcc
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

clean:
	rm -f *.o
	rm -f $(TARGET)
